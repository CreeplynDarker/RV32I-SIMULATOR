#include "instruction.hpp"

// opcode (inst[6:0]) -> formato
Format format_of(uint32_t opcode) {
    switch (opcode) {
        case 0x33:                                  return Format::R; // add, sub, ...
        case 0x13: case 0x03: case 0x67: case 0x73: return Format::I; // addi, loads, jalr, ecall
        case 0x23:                                  return Format::S; // sb, sh, sw
        case 0x63:                                  return Format::B; // beq, bne, ...
        case 0x37: case 0x17:                       return Format::U; // lui, auipc
        case 0x6F:                                  return Format::J; // jal
        default:                                    return Format::I; // desconocido
    }
}

// Reconstruye el inmediato segun el formato (tu unidad Extend / ImmSrc).
// Clave de RISC-V: el bit de signo SIEMPRE es inst[31]. El idioma
// (int32_t)(u << k) >> k coloca ese bit en la posicion 31 y luego
// el shift aritmetico a la derecha lo propaga -> extension de signo.
int32_t extract_imm(uint32_t raw, Format fmt) {
    switch (fmt) {
        case Format::I:  // imm[11:0] = inst[31:20]
            return static_cast<int32_t>(raw) >> 20;

        case Format::S: { // imm[11:5]=inst[31:25], imm[4:0]=inst[11:7]
            uint32_t u = (((raw >> 25) & 0x7F) << 5)
                       |  ((raw >> 7)  & 0x1F);
            return static_cast<int32_t>(u << 20) >> 20; // signo desde bit 11
        }

        case Format::B: { // imm[12|11|10:5|4:1], bit0 = 0
            uint32_t u = (((raw >> 31) & 0x1)  << 12)
                       | (((raw >> 7)  & 0x1)  << 11)
                       | (((raw >> 25) & 0x3F) << 5)
                       | (((raw >> 8)  & 0xF)  << 1);
            return static_cast<int32_t>(u << 19) >> 19; // signo desde bit 12
        }

        case Format::U:  // imm[31:12] = inst[31:12], 12 bits bajos en 0
            return static_cast<int32_t>(raw & 0xFFFFF000);

        case Format::J: { // imm[20|19:12|11|10:1], bit0 = 0
            uint32_t u = (((raw >> 31) & 0x1)   << 20)
                       | (((raw >> 12) & 0xFF)  << 12)
                       | (((raw >> 20) & 0x1)   << 11)
                       | (((raw >> 21) & 0x3FF) << 1);
            return static_cast<int32_t>(u << 11) >> 11; // signo desde bit 20
        }

        case Format::R:
        default:
            return 0; // R-type no tiene inmediato
    }
}

Instruction decode(uint32_t raw) {
    Instruction in{};
    in.raw    = raw;
    in.opcode =  raw        & 0x7F;
    in.rd     = (raw >> 7)  & 0x1F;
    in.funct3 = (raw >> 12) & 0x7;
    in.rs1    = (raw >> 15) & 0x1F;
    in.rs2    = (raw >> 20) & 0x1F;
    in.funct7 = (raw >> 25) & 0x7F;
    in.format = format_of(in.opcode);
    in.imm    = extract_imm(raw, in.format);
    return in;
}