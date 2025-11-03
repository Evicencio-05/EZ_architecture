#pragma once

#include "types.hpp"

namespace ez_arch {

enum class ALUOperation {
    ADD,
    SUB,
    AND,
    OR,
    SLT,  // Set less than
    NOR
};

class ALU {
public:
    struct Result {
        word_t value;
        bool carry;
        bool zero;
        bool overflow;
        bool negative;
    };
    
    static Result execute(ALUOperation op, word_t operand1, word_t operand2);
    
private:
    static bool check_overflow(word_t a, word_t b, word_t result, bool is_add);
};

} // namespace ez_arch
