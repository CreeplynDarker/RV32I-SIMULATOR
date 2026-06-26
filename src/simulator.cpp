#include "simulator.hpp"
#include "instruction.hpp"
#include <cstdio>

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
    uint32_t raw = fetch();
    Instruction in = decode(raw);

    // --- TEMPORAL (Fase 3): verificar el decode; se quita en la Fase 4 ---
    std::printf("  [decode] op=0x%02X rd=x%u f3=0x%X rs1=x%u rs2=x%u f7=0x%02X imm=%d (0x%X)\n",
                in.opcode, in.rd, in.funct3, in.rs1, in.rs2, in.funct7,
                in.imm, static_cast<uint32_t>(in.imm));

    pc_ += 4; // execute(in) entra en la Fase 4
}

/*void Simulator::step() {
    uint32_t instr = fetch();
    (void)instr;                   // todavia no decodificamos (Fase 3)
    pc_ += 4;                      // stub: avanza al siguiente word
}*/