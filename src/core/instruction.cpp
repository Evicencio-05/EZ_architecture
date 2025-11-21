#include "core/instruction.hpp"
#include <sstream>
#include <iomanip>

namespace ez_arch {

Instruction::Instruction(word_t raw) : m_raw(raw) {}

InstructionFormat Instruction::getFormat() const {
  uint8_t opcode = getOpcode();

  if (opcode == 0) {
    return InstructionFormat::R_TYPE;
  } if (opcode == 2 || opcode == 3) {
    return InstructionFormat::J_TYPE;
  }     return InstructionFormat::I_TYPE;
 
}

uint8_t Instruction::getOpcode() const {
  return (m_raw >> 26) & 0x3F;
}

register_id_t Instruction::getRs() const {
  return (m_raw >> 21) & 0x1F;
}

register_id_t Instruction::getRt() const {
  return (m_raw >> 16) & 0x1F;
}

register_id_t Instruction::getRd() const {
  return (m_raw >> 11) & 0x1F;
}

uint8_t Instruction::getShamt() const {
  return (m_raw >> 6) & 0x1F;
}

uint8_t Instruction::getFunct() const {
  return m_raw & 0x3F;
}

int16_t Instruction::getImmediate() const {
  return static_cast<int16_t>(m_raw & 0xFFFF);
}

uint32_t Instruction::getAddress() const {
  return m_raw & 0x3FFFFFF;
}

std::string Instruction::toString() const {
  std::stringstream ss;
  ss << "0x" << std::hex << std::setw(8) << std::setfill('0') << m_raw;
  ss << " [opcode = " << std::dec << static_cast<int>(getOpcode());

  switch (getFormat()) {
    case InstructionFormat::R_TYPE:
      ss << ", rs = " << static_cast<int>(getRs())
          << ", rt = " << static_cast<int>(get_rt())
          << ", rd = " << static_cast<int>(getRd())
          << ", shamt = " << static_cast<int>(getShamt())
          << ", funct = " << static_cast<int>(getFunct()) << "]";
      break;
    
    case InstructionFormat::I_TYPE:
      ss << ", rs = " << static_cast<int>(getRs())
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
