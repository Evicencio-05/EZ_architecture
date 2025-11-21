#pragma once

#include "types.hpp"
#include <array>

namespace ez_arch {

class RegisterFile {
public:
    RegisterFile();
    
    [[nodiscard]] word_t read(register_id_t reg) const;
    void write(register_id_t reg, word_t value);
    void reset();
    
    // Special registers
    [[nodiscard]] word_t getPc() const { return m_pc; }
    void setPc(word_t value) { m_pc = value; }
    void incrementPc() { m_pc += 4; }
    
    static constexpr std::size_t kNUM_REGISTERS = 32;
    
private:
    std::array<word_t, kNUM_REGISTERS> m_registers;
    word_t m_pc; // Program counter
};

} // namespace ez_arch
