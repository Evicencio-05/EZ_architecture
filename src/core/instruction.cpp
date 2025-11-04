#include "core/instruction.hpp"
#include <sstream>
#include <iomanip>

namespace ez_arch {

Instruction::Instruction(word_t raw) : raw_(raw) {}

InstructionFormat Instruction::get_format() const {
  uint8_t opcode = get_opcode();

  if (opcode == 0) {
    return InstructionFormat::R_TYPE;
  } else if (opcode == 2 || opcode == 3) {
    return InstructionFormat::J_TYPE;
  } else {
    return InstructionFormat::I_TYPE;
  }
}

uint8_t Instruction::get_opcode() const {
  return (raw_ >> 26) & 0x3F;
}

register_id_t Instruction::get_rs() const {
  return (raw_ >> 21) & 0x1F;
}

register_id_t Instruction::get_rt() const {
  return (raw_ >> 16) & 0x1F;
}

register_id_t Instruction::get_rd() const {
  return (raw_ >> 11) & 0x1F;
}

uint8_t Instruction::get_shamt() const {
  return (raw_ >> 6) & 0x1F;
}

uint8_t Instruction::get_funct() const {
  return raw_ & 0x3F;
}

int16_t Instruction::get_immediate() const {
  return static_cast<int16_t>(raw_ & 0xFFFF);
}

uint32_t Instruction::get_address() const {
  return raw_ & 0x3FFFFFF;
}

std::string Instruction::to_string() const {
  std::stringstream ss;
  ss << "0x" << std::hex << std::setw(8) << std::setfill('0') << raw_;
  ss << " [opcode = " << std::dec << static_cast<int>(get_opcode());

  switch (get_format()) {
    case InstructionFormat::R_TYPE:
      ss << ", rs = " << static_cast<int>(get_rs())
          << ", rt = " << static_cast<int>(get_rt())
          << ", rd = " << static_cast<int>(get_rd())
          << ", shamt = " << static_cast<int>(get_shamt())
          << ", funct = " << static_cast<int>(get_funct()) << "]";
      break;
    
    case InstructionFormat::I_TYPE:
      ss << ", rs = " << static_cast<int>(get_rs())
          << ", rt = " << static_cast<int>(get_rt())
          << ", imm = " << get_immediate() << "]";
      break;

    case InstructionFormat::J_TYPE:
      ss << ", addr = 0x" << std::hex << get_address() << "]";
      break;
  }

  return ss.str();
}

} // namespace ez_arch
