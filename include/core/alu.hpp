#pragma once

#include "types.hpp"

#include <cstdint>

namespace ez_arch {

enum class ALUOperation : uint8_t {
  ADD,
  SUB,
  AND,
  OR,
  SLT, // Set less than
  NOR
};

class ALU {
public:
  struct Result {
    word_t value;
    bool zero;
    bool overflow;
    bool negative;
  };

  static Result execute(ALUOperation operation,
                        word_t operand1,
                        word_t operand2);

private:
  static bool checkOverflow(word_t first,
                            word_t second,
                            word_t result,
                            bool isAdd);
};

} // namespace ez_arch
