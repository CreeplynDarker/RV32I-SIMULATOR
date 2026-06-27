#include "simulator.hpp"
#include "instruction.hpp"

/*void Simulator::load_program(const std::string& path) {
    mem_.load_raw(path);
}*/

void Simulator::load_program(const std::string& path) {
    mem_.load_raw(path);
    pc_ = 0;
    regs_[2] = 0x00FFFFF0;   // sp cerca del tope de los 16 MB (como CPUlator)
}

uint32_t Simulator::read_reg(int i) const {
    if (i == 0) return 0;
    return regs_[i];
}

void Simulator::write_reg(int i, uint32_t value) {
    if (i == 0) return;
    regs_[i] = value;
}

uint32_t Simulator::fetch() {
    return mem_.read32(pc_);
}

void Simulator::step() {
    uint32_t raw = fetch();
    Instruction in = decode(raw);

    uint32_t next_pc = pc_ + 4;   // por defecto
    execute(in, next_pc);         // puede modificar next_pc (desde Fase 6)
    pc_ = next_pc;
}