#include "core/alu.hpp"

namespace ez_arch {

ALU::Result ALU::execute(ALUOperation operation,
                         word_t operand1,
                         word_t operand2) {
  Result result;
  result.overflow = false; // Default to no overflow

  switch (operation) {
  case ALUOperation::ADD:
    result.value = operand1 + operand2;
    result.overflow = checkOverflow(operand1, operand2, result.value, true);
    break;

  case ALUOperation::SUB:
    result.value = operand1 - operand2;
    result.overflow = checkOverflow(operand1, operand2, result.value, false);
    break;

  case ALUOperation::AND:
    result.value = operand1 & operand2;
    result.overflow = false;
    break;

  case ALUOperation::OR:
    result.value = operand1 | operand2;
    result.overflow = false;
    break;

  case ALUOperation::SLT: {
    auto signedOp1 = static_cast<int32_t>(operand1);
    auto signedOp2 = static_cast<int32_t>(operand2);
    result.value = (signedOp1 < signedOp2) ? 1 : 0;
    result.overflow = false;
    break;
  }

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

bool ALU::checkOverflow(word_t first,
                        word_t second,
                        word_t result,
                        bool isAdd) {
  bool signA = ((first >> 31) & 1) != 0U;
  bool signB = ((second >> 31) & 1) != 0U;
  bool signResult = ((result >> 31) & 1) != 0U;

  if (isAdd) { return (signA == signB) && (signA != signResult); }
  return (signA != signB) && (signA != signResult);
}

} // namespace ez_arch
