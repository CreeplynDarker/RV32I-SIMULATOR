// instruction.hpp
#ifndef RV32I_INSTRUCTION_HPP
#define RV32I_INSTRUCTION_HPP

#include <cstdint>

enum class Format { R, I, S, B, U, J };

struct Instruction {
    uint32_t raw    = 0;
    uint32_t opcode = 0;
    uint32_t rd     = 0;
    uint32_t funct3 = 0;
    uint32_t rs1    = 0;
    uint32_t rs2    = 0;
    uint32_t funct7 = 0;
    int32_t  imm    = 0;   // ya extendido en signo
    Format   format = Format::I;
};

Format      format_of(uint32_t opcode);
int32_t     extract_imm(uint32_t raw, Format fmt);
Instruction decode(uint32_t raw);

#endif // RV32I_INSTRUCTION_HPP