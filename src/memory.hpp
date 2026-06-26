// memory.hpp
#ifndef RV32I_MEMORY_HPP
#define RV32I_MEMORY_HPP

#include <cstdint>
#include <string>
#include <vector>

class Memory {
public:
    // 16 MB de espacio plano: cubre 0x0 (programa),
    // 0x1000 (datos de prueba) y 0x64 (riscvtest).
    static constexpr uint32_t SIZE = 16u * 1024 * 1024;

    Memory();

    // Lecturas little-endian
    uint8_t  read8(uint32_t addr) const;
    uint16_t read16(uint32_t addr) const;
    uint32_t read32(uint32_t addr) const;

    // Escrituras little-endian
    void write8(uint32_t addr, uint8_t  value);
    void write16(uint32_t addr, uint16_t value);
    void write32(uint32_t addr, uint32_t value);

    // Carga un binario crudo a partir del address 0x0
    void load_raw(const std::string& path);

private:
    std::vector<uint8_t> data_;

    void check_bounds(uint32_t addr, uint32_t n_bytes) const;
};

#endif // RV32I_MEMORY_HPP
