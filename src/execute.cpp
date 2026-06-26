#include "simulator.hpp"
#include "instruction.hpp"
#include <cstdint>
#include <stdexcept>

void Simulator::execute(const Instruction& in, uint32_t& next_pc) {
    switch (in.opcode) {

    // ---------- R-type (0x33): rd = rs1 OP rs2 ----------
    case 0x33: {
        uint32_t a = read_reg(in.rs1);
        uint32_t b = read_reg(in.rs2);
        uint32_t shamt = b & 0x1F;          // solo 5 bits bajos
        uint32_t result = 0;
        switch (in.funct3) {
            case 0x0: result = (in.funct7 == 0x20) ? (a - b) : (a + b); break;  // sub / add
            case 0x1: result = a << shamt; break;                               // sll
            case 0x2: result = (int32_t(a) <  int32_t(b)) ? 1u : 0u; break;     // slt  (signed)
            case 0x3: result = (a <  b)                   ? 1u : 0u; break;     // sltu (unsigned)
            case 0x4: result = a ^ b; break;                                    // xor
            case 0x5: result = (in.funct7 == 0x20)
                             ? uint32_t(int32_t(a) >> shamt)   // sra (aritmetico)
                             : (a >> shamt);                   // srl (logico)
                      break;
            case 0x6: result = a | b; break;                                    // or
            case 0x7: result = a & b; break;                                    // and
            default:  throw std::runtime_error("R-type funct3 invalido");
        }
        write_reg(in.rd, result);
        break;
    }

    // ---------- I-type ALU (0x13): rd = rs1 OP imm ----------
    case 0x13: {
        uint32_t a   = read_reg(in.rs1);
        int32_t  imm = in.imm;                       // ya extendido en signo
        uint32_t shamt = uint32_t(imm) & 0x1F;
        uint32_t result = 0;
        switch (in.funct3) {
            case 0x0: result = a + uint32_t(imm); break;                        // addi
            case 0x1: result = a << shamt; break;                               // slli
            case 0x2: result = (int32_t(a) <  imm)           ? 1u : 0u; break;  // slti  (signed)
            case 0x3: result = (a <  uint32_t(imm))          ? 1u : 0u; break;  // sltiu (unsigned)
            case 0x4: result = a ^ uint32_t(imm); break;                        // xori
            case 0x5: result = (in.funct7 == 0x20)
                             ? uint32_t(int32_t(a) >> shamt)  // srai
                             : (a >> shamt);                  // srli
                      break;
            case 0x6: result = a | uint32_t(imm); break;                        // ori
            case 0x7: result = a & uint32_t(imm); break;                        // andi
            default:  throw std::runtime_error("I-type funct3 invalido");
        }
        write_reg(in.rd, result);
        break;
    }

    default:
        throw std::runtime_error("opcode no implementado todavia");
    }
}
