#pragma once

#include "types.hpp"
#include <string>

namespace ez_arch {

class Instruction {
public:
    explicit Instruction(word_t raw);
    
    InstructionFormat get_format() const;
    uint8_t get_opcode() const;
    
    // R-type fields
    register_id_t get_rs() const;
    register_id_t get_rt() const;
    register_id_t get_rd() const;
    uint8_t get_shamt() const;
    uint8_t get_funct() const;
    
    // I-type fields
    int16_t get_immediate() const;
    
    // J-type fields
    uint32_t get_address() const;
    
    word_t get_raw() const { return raw_; }
    std::string to_string() const;
    
private:
    word_t raw_;
};

} // namespace ez_arch
