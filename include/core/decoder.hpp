#pragma once

#include "types.hpp"
#include <string>
#include <vector>

namespace ez_arch {

class Decoder {
public:
    // Decode binary instruction to human-readable format
    static std::string decode(word_t instruction);
    
    // Assembly to machine code
    static word_t assemble(const std::string& assembly_line);
    
    // Get instruction details for educational display
    struct InstructionDetails {
        std::string mnemonic;
        InstructionFormat format;
        std::vector<std::string> fields;
        std::string description;
    };
    
    static InstructionDetails get_details(word_t instruction);
};

} // namespace ez_arch
