#include "instruction.hpp"

#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <string>

// Nombres ABI de los registros
static const char* RN[32] = {
    "zero","ra","sp","gp","tp","t0","t1","t2",
    "s0","s1","a0","a1","a2","a3","a4","a5",
    "a6","a7","s2","s3","s4","s5","s6","s7",
    "s8","s9","s10","s11","t3","t4","t5","t6"
};

// Helper tipo printf que devuelve un std::string
static std::string fmt(const char* f, ...) {
    char tmp[128];
    va_list ap; va_start(ap, f);
    std::vsnprintf(tmp, sizeof(tmp), f, ap);
    va_end(ap);
    return std::string(tmp);
}

std::string disassemble(const Instruction& in, uint32_t pc) {
    switch (in.opcode) {

    case 0x33: { // R-type
        const char* m = "?";
        switch (in.funct3) {
            case 0x0: m = (in.funct7 == 0x20) ? "sub" : "add"; break;
            case 0x1: m = "sll";  break;
            case 0x2: m = "slt";  break;
            case 0x3: m = "sltu"; break;
            case 0x4: m = "xor";  break;
            case 0x5: m = (in.funct7 == 0x20) ? "sra" : "srl"; break;
            case 0x6: m = "or";   break;
            case 0x7: m = "and";  break;
        }
        return fmt("%-5s %s, %s, %s", m, RN[in.rd], RN[in.rs1], RN[in.rs2]);
    }

    case 0x13: { // I-type ALU
        const char* m = "?";
        switch (in.funct3) {
            case 0x0: m = "addi";  break;
            case 0x1: m = "slli";  break;
            case 0x2: m = "slti";  break;
            case 0x3: m = "sltiu"; break;
            case 0x4: m = "xori";  break;
            case 0x5: m = (in.funct7 == 0x20) ? "srai" : "srli"; break;
            case 0x6: m = "ori";   break;
            case 0x7: m = "andi";  break;
        }
        if (in.funct3 == 0x1 || in.funct3 == 0x5)  // shifts: el operando es shamt
            return fmt("%-5s %s, %s, %d", m, RN[in.rd], RN[in.rs1], in.imm & 0x1F);
        return fmt("%-5s %s, %s, %d", m, RN[in.rd], RN[in.rs1], in.imm);
    }

    case 0x03: { // loads
        const char* m = "?";
        switch (in.funct3) {
            case 0x0: m="lb"; break; case 0x1: m="lh"; break; case 0x2: m="lw"; break;
            case 0x4: m="lbu"; break; case 0x5: m="lhu"; break;
        }
        return fmt("%-5s %s, %d(%s)", m, RN[in.rd], in.imm, RN[in.rs1]);
    }

    case 0x23: { // stores
        const char* m = "?";
        switch (in.funct3) { case 0x0: m="sb"; break; case 0x1: m="sh"; break; case 0x2: m="sw"; break; }
        return fmt("%-5s %s, %d(%s)", m, RN[in.rs2], in.imm, RN[in.rs1]);
    }

    case 0x63: { // branches
        const char* m = "?";
        switch (in.funct3) {
            case 0x0: m="beq"; break; case 0x1: m="bne"; break;
            case 0x4: m="blt"; break; case 0x5: m="bge"; break;
            case 0x6: m="bltu"; break; case 0x7: m="bgeu"; break;
        }
        return fmt("%-5s %s, %s, 0x%X", m, RN[in.rs1], RN[in.rs2], pc + uint32_t(in.imm));
    }

    case 0x6F: return fmt("%-5s %s, 0x%X", "jal", RN[in.rd], pc + uint32_t(in.imm));
    case 0x67: return fmt("%-5s %s, %d(%s)", "jalr", RN[in.rd], in.imm, RN[in.rs1]);
    case 0x37: return fmt("%-5s %s, 0x%X", "lui",   RN[in.rd], uint32_t(in.imm) >> 12);
    case 0x17: return fmt("%-5s %s, 0x%X", "auipc", RN[in.rd], uint32_t(in.imm) >> 12);

    case 0x73:
        if (in.raw == 0x00000073) return "ecall";
        if (in.raw == 0x00100073) return "ebreak";
        return "system";

    default:
        return fmt(".word 0x%08X", in.raw);  // bytes no decodificables (datos, ceros)
    }
}
