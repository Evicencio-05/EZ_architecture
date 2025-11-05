#pragma once

#include "types.hpp"
#include <string>
#include <vector>

namespace ez_arch {

class Decoder {
public:
    static std::string decode(word_t instruction);
    
    static word_t assemble(const std::string& assembly_line);
    
    struct InstructionDetails {
        std::string mnemonic;
        InstructionFormat format;
        std::vector<std::string> fields;
        std::string description;
    };
    
    static InstructionDetails get_details(word_t instruction);
};

} // namespace ez_arch
