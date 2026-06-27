#include <cstdint>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "simulator.hpp"

// Nombres ABI de los 32 registros
static const char* ABI[32] = {
    "zero","ra","sp","gp","tp","t0","t1","t2",
    "s0","s1","a0","a1","a2","a3","a4","a5",
    "a6","a7","s2","s3","s4","s5","s6","s7",
    "s8","s9","s10","s11","t3","t4","t5","t6"
};

static std::vector<std::string> split(const std::string& line) {
    std::vector<std::string> tokens;
    std::istringstream iss(line);
    std::string tok;
    while (iss >> tok) tokens.push_back(tok);
    return tokens;
}

static bool parse_uint(const std::string& s, uint32_t& out) {
    try { out = static_cast<uint32_t>(std::stoul(s, nullptr, 0)); return true; }
    catch (...) { return false; }
}

// Acepta "x5", "a0", "sp", "fp", "zero", etc.
static bool parse_reg(const std::string& s, int& out) {
    if (s.size() >= 2 && (s[0] == 'x' || s[0] == 'X')) {
        try {
            int n = std::stoi(s.substr(1));
            if (n >= 0 && n <= 31) { out = n; return true; }
        } catch (...) {}
    }
    if (s == "fp") { out = 8; return true; }   // alias de s0
    for (int i = 0; i < 32; ++i)
        if (s == ABI[i]) { out = i; return true; }
    return false;
}

static void print_reg(const Simulator& sim, int r) {
    std::printf("x%-2d (%-4s) = 0x%08X\n", r, ABI[r], sim.read_reg(r));
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <programa.bin>\n";
        return 1;
    }

    Simulator sim;
    try {
        sim.load_program(argv[1]);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    std::cout << "\"" << argv[1] << "\" cargado a memoria.\n";

    const uint32_t RUN_LIMIT = 10000000;  // tope de seguridad anti-bucle

    std::string line;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;

        std::vector<std::string> args = split(line);
        if (args.empty()) continue;
        const std::string& cmd = args[0];

        if (cmd == "exit" || cmd == "quit") {
            std::cout << "See you next time...\n";
            break;
        }
        else if (cmd == "pc") {
            std::printf("pc = 0x%08X\n", sim.pc());
        }
        else if (cmd == "step" || cmd == "s") {
            uint32_t n = 1;
            if (args.size() >= 2 && !parse_uint(args[1], n)) {
                std::cout << "Uso: step [N]\n";
                continue;
            }
            uint32_t done = 0;
            try {
                for (; done < n && !sim.halted(); ++done) sim.step();
            } catch (const std::exception& e) {
                std::printf("Error de ejecucion en 0x%08X: %s\n", sim.pc(), e.what());
            }
            if (n == 1) std::cout << "Ejecutando instruccion.\n";
            else std::printf("Ejecutadas %u instrucciones (pc = 0x%08X).\n", done, sim.pc());
        }
        else if (cmd == "run" || cmd == "continue" || cmd == "c") {
            uint32_t count = 0;
            try {
                while (!sim.halted() && count < RUN_LIMIT) {
                    uint32_t before = sim.pc();
                    sim.step();
                    ++count;
                    if (sim.pc() == before) break;   // salto a si misma -> fin
                }
            } catch (const std::exception& e) {
                std::printf("Error de ejecucion en 0x%08X: %s\n", sim.pc(), e.what());
                continue;
            }
            if (count >= RUN_LIMIT)
                std::printf("Limite de %u instrucciones (posible bucle infinito).\n", RUN_LIMIT);
            else
                std::printf("Detenido en pc = 0x%08X (%u instrucciones).\n", sim.pc(), count);
        }
        else if (cmd == "regs" || cmd == "reg" || cmd == "r") {
            if (args.size() < 2) {
                for (int i = 0; i < 32; ++i) print_reg(sim, i);   // todos
            } else {
                for (size_t i = 1; i < args.size(); ++i) {
                    int r;
                    if (parse_reg(args[i], r)) print_reg(sim, r);
                    else std::cout << "Registro invalido: " << args[i] << "\n";
                }
            }
        }
        else if (cmd == "mem" || cmd == "m") {
            uint32_t start, end;
            if (args.size() < 3 || !parse_uint(args[1], start) || !parse_uint(args[2], end)) {
                std::cout << "Uso: mem <inicio> <fin>  (ej: mem 0x1000 0x100F)\n";
            } else if (end < start) {
                std::cout << "El fin debe ser >= el inicio.\n";
            } else {
                try {
                    for (uint32_t base = start; base <= end; base += 16) {
                        std::printf("0x%08X:", base);
                        std::string ascii;
                        for (uint32_t off = 0; off < 16; ++off) {
                            uint32_t a = base + off;
                            if (a > end) { std::printf("   "); ascii += ' '; }
                            else {
                                uint8_t b = sim.memory().read8(a);
                                std::printf(" %02X", b);
                                ascii += (b >= 0x20 && b < 0x7F) ? char(b) : '.';
                            }
                        }
                        std::printf("  |%s|\n", ascii.c_str());
                    }
                } catch (const std::exception& e) {
                    std::printf("\n%s\n", e.what());
                }
            }
        }
        else if (cmd == "help" || cmd == "h" || cmd == "?") {
            std::cout <<
                "Comandos:\n"
                "  pc                  muestra el PC\n"
                "  step [N] | s        ejecuta 1 (o N) instrucciones\n"
                "  run | continue | c  ejecuta hasta el final (o bucle propio)\n"
                "  regs [rN ...] | r   muestra registros (todos, o los pedidos)\n"
                "  mem <ini> <fin>     vuelca memoria en hex + ASCII\n"
                "  help | exit\n";
        }
        else {
            std::cout << "Comando desconocido: " << cmd << " (escribe 'help')\n";
        }
    }

    return sim.exit_code();
}
