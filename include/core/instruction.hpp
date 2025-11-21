#pragma once

#include "types.hpp"

#include <string>

namespace ez_arch {

class Instruction {
public:
  explicit Instruction(word_t raw);

  [[nodiscard]] InstructionFormat getFormat() const;
  [[nodiscard]] uint8_t getOpcode() const;

  // R-type fields
  [[nodiscard]] register_id_t getRs() const;
  [[nodiscard]] register_id_t getRt() const;
  [[nodiscard]] register_id_t getRd() const;
  [[nodiscard]] uint8_t getShamt() const;
  [[nodiscard]] uint8_t getFunct() const;

  // I-type fields
  [[nodiscard]] int16_t getImmediate() const;

  // J-type fields
  [[nodiscard]] uint32_t getAddress() const;

  [[nodiscard]] word_t getRaw() const { return m_raw; }
  [[nodiscard]] std::string toString() const;

private:
  word_t m_raw;
};

} // namespace ez_arch
