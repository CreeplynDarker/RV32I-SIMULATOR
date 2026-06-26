// simulator.hpp
#ifndef RV32I_SIMULATOR_HPP
#define RV32I_SIMULATOR_HPP

#include <cstdint>
#include <string>
#include "memory.hpp"

class Simulator {
public:
    Simulator() = default;

    // Carga un binario crudo al address 0x0
    void load_program(const std::string& path);

    // Ejecuta una instruccion (por ahora solo avanza el PC)
    void step();

    // --- Acceso al estado (para el REPL) ---
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
};

#endif // RV32I_SIMULATOR_HPP