// simulator.hpp
#ifndef RV32I_SIMULATOR_HPP
#define RV32I_SIMULATOR_HPP

#include <cstdint>
#include <string>
#include "memory.hpp"

struct Instruction;   // <-- forward declaration

class Simulator {
public:
    Simulator() = default;
    void load_program(const std::string& path);
    void step();

    uint32_t pc() const { return pc_; }
    uint32_t read_reg(int i) const;
    void     write_reg(int i, uint32_t value);

    bool halted() const { return halted_; }
    int  exit_code() const { return exit_code_; }
    const Memory& memory() const { return mem_; }

private:
    uint32_t pc_ = 0;
    uint32_t regs_[32] = {};
    Memory   mem_;
    bool     halted_ = false;
    int      exit_code_ = 0;

    uint32_t fetch();
    void execute(const Instruction& in, uint32_t& next_pc);  // <-- nuevo
};

#endif // RV32I_SIMULATOR_HPP