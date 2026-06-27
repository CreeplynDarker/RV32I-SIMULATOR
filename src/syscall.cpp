#include "simulator.hpp"

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>

// Indices ABI de los registros que usan las syscalls
static constexpr int A0 = 10;   // x10
static constexpr int A1 = 11;   // x11
static constexpr int A7 = 17;   // x17

void Simulator::handle_ecall() {
    uint32_t service = read_reg(A7);   // numero de syscall en a7
    uint32_t a0      = read_reg(A0);   // argumento en a0

    switch (service) {
        case 1:  // print_int (con signo)
            std::printf("%d", int32_t(a0));
            std::fflush(stdout);
            break;

        case 4: { // print_string: a0 = puntero a cadena terminada en '\0'
            std::string s;
            for (uint32_t addr = a0; ; ++addr) {
                uint8_t c = mem_.read8(addr);
                if (c == 0) break;
                s += char(c);
            }
            std::printf("%s", s.c_str());
            std::fflush(stdout);
            break;
        }

        case 5: { // read_int -> a0
            int32_t v = 0;
            std::cin >> v;
            write_reg(A0, uint32_t(v));
            break;
        }

        case 8: { // read_string: a0 = buffer, a1 = max chars
            uint32_t buf = a0, maxn = read_reg(A1);
            std::string line;
            std::getline(std::cin, line);
            uint32_t i = 0;
            for (; i + 1 < maxn && i < line.size(); ++i)
                mem_.write8(buf + i, uint8_t(line[i]));
            if (maxn > 0) mem_.write8(buf + i, 0); // terminador nulo
            break;
        }

        case 10: // exit
            halted_ = true;
            exit_code_ = 0;
            break;

        case 11: // print_char
            std::printf("%c", char(a0 & 0xFF));
            std::fflush(stdout);
            break;

        case 12: { // read_char -> a0
            int c = std::getchar();
            write_reg(A0, uint32_t(int32_t(c)));
            break;
        }

        case 17: // exit2 (a0 = codigo de salida)
            halted_ = true;
            exit_code_ = int(a0);
            break;

        case 34: std::printf("0x%X", a0);  std::fflush(stdout); break; // print hex
        case 36: std::printf("%u", a0);    std::fflush(stdout); break; // print unsigned

        default:
            std::printf("[ecall no soportado: a7=%u]\n", service);
            break;
    }
}
