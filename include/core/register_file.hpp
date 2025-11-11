#pragma once

#include "types.hpp"
#include <array>

namespace ez_arch {

class RegisterFile {
public:
    RegisterFile();
    
    word_t read(register_id_t reg) const;
    void write(register_id_t reg, word_t value);
    void reset();
    
    // Special registers
    word_t get_pc() const { return m_pc; }
    void set_pc(word_t value) { m_pc = value; }
    void increment_pc() { m_pc += 4; }
    
    static constexpr size_t NUM_REGISTERS = 32;
    
private:
    std::array<word_t, NUM_REGISTERS> m_registers;
    word_t m_pc; // Program counter
};

} // namespace ez_arch
