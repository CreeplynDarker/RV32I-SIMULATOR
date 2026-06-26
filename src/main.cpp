#include <cstdio>
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
}

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