#include <stdexcept>
#include <iostream>
#include <algorithm>

#include "core/register_file.hpp"

using namespace ez_arch;

word_t RegisterFile::read(register_id_t reg) const {
  try {
    if (reg < NUM_REGISTERS && reg >= 0) {
      return registers_[reg];
    } else {
      throw std::range_error("Register number out of range.") 
    }
  }
  catch (const std::range_error& e) {
    std::cerr << "Exception caught: " << e.what() << '\n';
  }
}

void write(register_id_t reg, word_t value) {
  if (reg == 0) {
    std::cerr << "Cannot write to $zero register." << '\n';
  } else {
    try {
      if (reg < NUM_REGISTERS && reg > 0) {
        registers_[reg] = value;
      } else {
        throw std::range_error("Register number out of range.") 
      }
    }
    catch (const std::range_error& e) {
      std::cerr << "Exception caught: " << e.what() << '\n';
    }
  }
}

void reset() {
  registers_.fill(0);
  pc_ = 0;
}
