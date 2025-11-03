#pragma once

#include "types.hpp"
#include <array>

namespace ez_arch {

class RegisterFile {
public:
    RegisterFile() : 
      registers_(registers_.fill(0)), pc_(0);
    
    word_t read(register_id_t reg) const;
    void write(register_id_t reg, word_t value);
    void reset();
    
    // Special registers
    word_t get_pc() const { return pc_; }
    void set_pc(word_t value) { pc_ = value; }
    void increment_pc() { pc_ += 4; }
    
    static constexpr size_t NUM_REGISTERS = 32;
    
private:
    std::array<word_t, NUM_REGISTERS> registers_;
    word_t pc_; // Program counter
};

} // namespace ez_arch
