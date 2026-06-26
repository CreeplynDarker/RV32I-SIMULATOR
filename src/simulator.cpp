#include "simulator.hpp"

void Simulator::load_program(const std::string& path) {
    mem_.load_raw(path);
}

uint32_t Simulator::read_reg(int i) const {
    if (i == 0) return 0;          // x0 siempre lee 0
    return regs_[i];
}

void Simulator::write_reg(int i, uint32_t value) {
    if (i == 0) return;            // escrituras a x0 se descartan
    regs_[i] = value;
}

uint32_t Simulator::fetch() {
    return mem_.read32(pc_);       // little-endian, ya resuelto en Memory
}

void Simulator::step() {
    uint32_t instr = fetch();
    (void)instr;                   // todavia no decodificamos (Fase 3)
    pc_ += 4;                      // stub: avanza al siguiente word
}