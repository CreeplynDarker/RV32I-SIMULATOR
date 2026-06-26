#include "memory.hpp"

#include <fstream>
#include <stdexcept>

Memory::Memory() : data_(SIZE, 0) {}

void Memory::check_bounds(uint32_t addr, uint32_t n_bytes) const {
    if (addr + n_bytes > SIZE) {
        throw std::out_of_range("Acceso a memoria fuera de rango");
    }
}

// ---------- Lecturas (little-endian) ----------

uint8_t Memory::read8(uint32_t addr) const {
    check_bounds(addr, 1);
    return data_[addr];
}

uint16_t Memory::read16(uint32_t addr) const {
    check_bounds(addr, 2);
    return  static_cast<uint16_t>(data_[addr])
         | (static_cast<uint16_t>(data_[addr + 1]) << 8);
}

uint32_t Memory::read32(uint32_t addr) const {
    check_bounds(addr, 4);
    return  static_cast<uint32_t>(data_[addr])
         | (static_cast<uint32_t>(data_[addr + 1]) << 8)
         | (static_cast<uint32_t>(data_[addr + 2]) << 16)
         | (static_cast<uint32_t>(data_[addr + 3]) << 24);
}

// ---------- Escrituras (little-endian) ----------

void Memory::write8(uint32_t addr, uint8_t value) {
    check_bounds(addr, 1);
    data_[addr] = value;
}

void Memory::write16(uint32_t addr, uint16_t value) {
    check_bounds(addr, 2);
    data_[addr]     =  value        & 0xFF;
    data_[addr + 1] = (value >> 8)  & 0xFF;
}

void Memory::write32(uint32_t addr, uint32_t value) {
    check_bounds(addr, 4);
    data_[addr]     =  value        & 0xFF;
    data_[addr + 1] = (value >> 8)  & 0xFF;
    data_[addr + 2] = (value >> 16) & 0xFF;
    data_[addr + 3] = (value >> 24) & 0xFF;
}

// ---------- Carga de programa ----------

void Memory::load_raw(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("No se pudo abrir el archivo: " + path);
    }

    // Lee todo el archivo a partir del address 0x0
    file.read(reinterpret_cast<char*>(data_.data()), SIZE);
    // file.read deja de leer al llegar al EOF; el resto queda en 0.
}
