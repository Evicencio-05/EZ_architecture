#include "core/alu.hpp"

namespace ez_arch {

ALU::Result ALU::execute(ALUOperation op, word_t operand1, word_t operand2) {
    Result result;
    result.overflow = false;  // Default to no overflow
    
    switch(op) {
        case ALUOperation::ADD:
            result.value = operand1 + operand2;
            result.overflow = check_overflow(operand1, operand2, result.value, true);
            break;

        case ALUOperation::SUB:
            result.value = operand1 - operand2;
            result.overflow = check_overflow(operand1, operand2, result.value, false);
            break;
            
        case ALUOperation::AND:
            result.value = operand1 & operand2;
            result.overflow = false;
            break;
            
        case ALUOperation::OR:
            result.value = operand1 | operand2;
            result.overflow = false;
            break;
            
        case ALUOperation::SLT:
            int32_t signed_op1 = static_cast<int32_t>(operand1);
            int32_t signed_op2 = static_cast<int32_t>(operand2);
            result.value = (signed_op1 < signed_op2) ? 1 : 0;
            result.overflow = false;
            break;
            
        case ALUOperation::NOR:
            result.value = ~(operand1 | operand2);
            result.overflow = false;
            break;
    }
    
    // Set common flags
    result.zero = (result.value == 0);
    result.negative = (static_cast<int32_t>(result.value) < 0);
    
    return result;
}

bool ALU::check_overflow(word_t a, word_t b, word_t result, bool is_add) {
  bool op1_sign     = (a >> 31) & 1;
  bool op2_sign     = (b >> 31) & 1;
  bool result_sign  = (result >> 31) & 1;

  if (is_add) {
        return (sign_a == sign_b) && (sign_a != sign_result);
    } else {
        return (sign_a != sign_b) && (sign_a != sign_result);
    }
}

} // namespace ez_arch
