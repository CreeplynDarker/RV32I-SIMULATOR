#include <cstdint>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "simulator.hpp"

// Divide una linea en tokens separados por espacios
static std::vector<std::string> split(const std::string& line) {
    std::vector<std::string> tokens;
    std::istringstream iss(line);
    std::string tok;
    while (iss >> tok) tokens.push_back(tok);
    return tokens;
}

// Parsea un entero sin signo: acepta 0x.. (hex) o decimal
static bool parse_uint(const std::string& s, uint32_t& out) {
    try {
        out = static_cast<uint32_t>(std::stoul(s, nullptr, 0));
        return true;
    } catch (...) { return false; }
}

// Parsea un registro tipo "x5" -> 5
static bool parse_reg(const std::string& s, int& out) {
    if (s.size() < 2 || (s[0] != 'x' && s[0] != 'X')) return false;
    try {
        int n = std::stoi(s.substr(1));
        if (n < 0 || n > 31) return false;
        out = n;
        return true;
    } catch (...) { return false; }
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

    std::string line;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;   // EOF (Ctrl+D / Ctrl+Z)

        std::vector<std::string> args = split(line);
        if (args.empty()) continue;
        const std::string& cmd = args[0];

        if (cmd == "exit") {
            std::cout << "See you next time...\n";
            break;
        }
        else if (cmd == "pc") {
            std::printf("pc = 0x%08X\n", sim.pc());
        }
        else if (cmd == "step") {
            try {
                sim.step();
                std::cout << "Ejecutando instruccion.\n";
            } catch (const std::exception& e) {
                std::cout << "Error de ejecucion: " << e.what() << "\n";
            }
        }
        else if (cmd == "regs") {
            if (args.size() < 2) {
                std::cout << "Uso: regs x5 x14 ...\n";
            } else {
                for (size_t i = 1; i < args.size(); ++i) {
                    int r;
                    if (parse_reg(args[i], r))
                        std::printf("x%d = 0x%08X\n", r, sim.read_reg(r));
                    else
                        std::cout << "Registro invalido: " << args[i] << "\n";
                }
            }
        }
        else if (cmd == "mem") {
            uint32_t start, end;
            if (args.size() < 3 || !parse_uint(args[1], start) || !parse_uint(args[2], end)) {
                std::cout << "Uso: mem <inicio> <fin>  (ej: mem 0x1000 0x1003)\n";
            } else if (end < start) {
                std::cout << "El fin debe ser >= el inicio.\n";
            } else {
                try {
                    std::printf("Memoria (0x%X-0x%X):", start, end);
                    for (uint32_t a = start; a <= end; ++a)
                        std::printf(" %02X", sim.memory().read8(a));
                    std::printf("\n");
                } catch (const std::exception& e) {
                    std::printf("\n%s\n", e.what());
                }
            }
        }
        else {
            std::cout << "Comando desconocido: " << cmd << "\n";
        }
    }

    return sim.exit_code();
}

// FASE 1 TEST
/*#include <cstdio>
#include "memory.hpp"

int main() {
    Memory mem;
    mem.write32(0x100, 0xDEADBEEF);

    // Esperado en little-endian: EF BE AD DE
    std::printf("bytes en 0x100: %02X %02X %02X %02X\n",
                mem.read8(0x100), mem.read8(0x101),
                mem.read8(0x102), mem.read8(0x103));

    // Round-trip: debe imprimir DEADBEEF
    std::printf("read32(0x100) = %08X\n", mem.read32(0x100));
    return 0;
}*/

// PHASE 0 TEST
/*#include <iostream>
#include <string>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <programa.bin>\n";
        return 1;
    }

    const std::string path = argv[1];
    std::cout << "rv32i-sim: archivo de entrada = " << path << "\n";
    std::cout << "(andamiaje listo, sin logica todavia)\n";
    return 0;
}*/