#include "simulator.hpp"
#include "instruction.hpp"
#include <cstdint>
#include <stdexcept>

void Simulator::execute(const Instruction& in, uint32_t& next_pc)
{
    switch (in.opcode)
    {
    // ---------- R-type (0x33): rd = rs1 OP rs2 ----------
    case 0x33:
        {
            uint32_t a = read_reg(in.rs1);
            uint32_t b = read_reg(in.rs2);
            uint32_t shamt = b & 0x1F; // solo 5 bits bajos
            uint32_t result = 0;
            switch (in.funct3)
            {
            case 0x0: result = (in.funct7 == 0x20) ? (a - b) : (a + b);
                break; // sub / add
            case 0x1: result = a << shamt;
                break; // sll
            case 0x2: result = (int32_t(a) < int32_t(b)) ? 1u : 0u;
                break; // slt  (signed)
            case 0x3: result = (a < b) ? 1u : 0u;
                break; // sltu (unsigned)
            case 0x4: result = a ^ b;
                break; // xor
            case 0x5: result = (in.funct7 == 0x20)
                                   ? uint32_t(int32_t(a) >> shamt) // sra (aritmetico)
                                   : (a >> shamt); // srl (logico)
                break;
            case 0x6: result = a | b;
                break; // or
            case 0x7: result = a & b;
                break; // and
            default: throw std::runtime_error("R-type funct3 invalido");
            }
            write_reg(in.rd, result);
            break;
        }

    // ---------- I-type ALU (0x13): rd = rs1 OP imm ----------
    case 0x13:
        {
            uint32_t a = read_reg(in.rs1);
            int32_t imm = in.imm; // ya extendido en signo
            uint32_t shamt = uint32_t(imm) & 0x1F;
            uint32_t result = 0;
            switch (in.funct3)
            {
            case 0x0: result = a + uint32_t(imm);
                break; // addi
            case 0x1: result = a << shamt;
                break; // slli
            case 0x2: result = (int32_t(a) < imm) ? 1u : 0u;
                break; // slti  (signed)
            case 0x3: result = (a < uint32_t(imm)) ? 1u : 0u;
                break; // sltiu (unsigned)
            case 0x4: result = a ^ uint32_t(imm);
                break; // xori
            case 0x5: result = (in.funct7 == 0x20)
                                   ? uint32_t(int32_t(a) >> shamt) // srai
                                   : (a >> shamt); // srli
                break;
            case 0x6: result = a | uint32_t(imm);
                break; // ori
            case 0x7: result = a & uint32_t(imm);
                break; // andi
            default: throw std::runtime_error("I-type funct3 invalido");
            }
            write_reg(in.rd, result);
            break;
        }

    // ---------- Loads (0x03): rd = MEM[rs1 + imm] ----------
    case 0x03:
        {
            uint32_t addr = read_reg(in.rs1) + uint32_t(in.imm);
            uint32_t result = 0;
            switch (in.funct3)
            {
            case 0x0: result = uint32_t(int32_t(int8_t(mem_.read8(addr))));
                break; // lb  (signo)
            case 0x1: result = uint32_t(int32_t(int16_t(mem_.read16(addr))));
                break; // lh  (signo)
            case 0x2: result = mem_.read32(addr);
                break; // lw
            case 0x4: result = uint32_t(mem_.read8(addr));
                break; // lbu (ceros)
            case 0x5: result = uint32_t(mem_.read16(addr));
                break; // lhu (ceros)
            default: throw std::runtime_error("load funct3 invalido");
            }
            write_reg(in.rd, result);
            break;
        }

    // ---------- Stores (0x23): MEM[rs1 + imm] = rs2 ----------
    case 0x23:
        {
            uint32_t addr = read_reg(in.rs1) + uint32_t(in.imm);
            uint32_t val = read_reg(in.rs2);
            switch (in.funct3)
            {
            case 0x0: mem_.write8(addr, uint8_t(val & 0xFF));
                break; // sb
            case 0x1: mem_.write16(addr, uint16_t(val & 0xFFFF));
                break; // sh
            case 0x2: mem_.write32(addr, val);
                break; // sw
            default: throw std::runtime_error("store funct3 invalido");
            }
            break;
        }

    // ---------- Branches (0x63): si se cumple, next_pc = pc + imm ----------
    case 0x63:
        {
            uint32_t a = read_reg(in.rs1);
            uint32_t b = read_reg(in.rs2);
            bool taken = false;
            switch (in.funct3)
            {
            case 0x0: taken = (a == b);
                break; // beq
            case 0x1: taken = (a != b);
                break; // bne
            case 0x4: taken = (int32_t(a) < int32_t(b));
                break; // blt  (signed)
            case 0x5: taken = (int32_t(a) >= int32_t(b));
                break; // bge  (signed)
            case 0x6: taken = (a < b);
                break; // bltu (unsigned)
            case 0x7: taken = (a >= b);
                break; // bgeu (unsigned)
            default: throw std::runtime_error("branch funct3 invalido");
            }
            if (taken) next_pc = pc_ + uint32_t(in.imm);
            break;
        }

    // ---------- jal (0x6F): rd = pc+4 ; next_pc = pc + imm ----------
    case 0x6F:
        {
            write_reg(in.rd, pc_ + 4);
            next_pc = pc_ + uint32_t(in.imm);
            break;
        }

    // ---------- jalr (0x67): rd = pc+4 ; next_pc = (rs1 + imm) & ~1 ----------
    case 0x67:
        {
            uint32_t target = (read_reg(in.rs1) + uint32_t(in.imm)) & ~1u; // calcula ANTES
            write_reg(in.rd, pc_ + 4); // de escribir rd
            next_pc = target;
            break;
        }

    // ---------- lui (0x37): rd = imm (12 bits bajos ya en 0) ----------
    case 0x37:
        write_reg(in.rd, uint32_t(in.imm));
        break;

    // ---------- auipc (0x17): rd = pc + imm ----------
    case 0x17:
        write_reg(in.rd, pc_ + uint32_t(in.imm));
        break;

    default:
        throw std::runtime_error("opcode no implementado todavia");
    }
}
