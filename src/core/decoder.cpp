#include "core/decoder.hpp"
#include "core/instruction.hpp"
#include "core/register_names.hpp"
#include <sstream>
#include <iomanip>

namespace ez_arch {

static std::string decode_r_type(const Instruction& instr) {
  uint8_t funct = instr.get_funct();
  uint8_t rs = instr.get_rs();
  uint8_t rt = instr.get_rt();
  uint8_t rd = instr.get_rd();

  std::string mnemonic;

  switch (funct) {
    case Funct::ADD: mnemonic = "add"; break;
    case Funct::SUB: mnemonic = "sub"; break;
    case Funct::AND: mnemonic = "and"; break;
    case Funct::OR: mnemonic = "or"; break;
    case Funct::SLT: mnemonic = "slt"; break;
    default: return "unknown";
  }

  std::stringstream ss;
  ss << mnemonic << " " << REGISTER_NAMES[rd] << ", "
      << REGISTER_NAMES[rs] << ", " << REGISTER_NAMES[rt];

  return ss.str();
}

static std::string decode_i_type(const Instruction& instr) {
  uint8_t opcode = instr.get_opcode();
  uint8_t rs = instr.get_rs();
  uint8_t rt = instr.get_rt();
  int16_t imm = instr.get_immediate();

  std::string mnemonic;

  switch (opcode) {
    case Opcode::ADDI: mnemonic = "addi"; break;
    case Opcode::ANDI: mnemonic = "andi"; break;
    case Opcode::ORI: mnemonic = "ori"; break;
    case Opcode::LW: mnemonic = "lw"; break;
    case Opcode::SW: mnemonic = "sw"; break;
    case Opcode::BEQ: mnemonic = "beq"; break;
    case Opcode::BNE: mnemonic = "bne"; break;
    default: return "unknown";
  }

  std::stringstream ss;

  switch (opcode) {
    case Opcode::SW: [[fallthrough]];
    case Opcode::LW:
      // Format: lw $rt, offset($rs)
      ss << mnemonic << " " << REGISTER_NAMES[rt] << ", " 
          << imm << "(" << REGISTER_NAMES[rs] << ")";
      break;

    case Opcode::BEQ: [[fallthrough]];
    case Opcode::BNE:
      // Format: beq $rs, $rt, offset
      ss << mnemonic << " " << REGISTER_NAMES[rs] << ", " 
           << REGISTER_NAMES[rt] << ", " << imm;
      break;

    default:
      // Format: addi $rt, $rs, immediate
      ss << mnemonic << " " << REGISTER_NAMES[rt] << ", " 
        << REGISTER_NAMES[rs] << ", " << imm;
      break;
  }

  return ss.str();
}

static std::string decode_j_type(const Instruction& instr) {
    uint8_t opcode = instr.get_opcode();
    uint32_t addr = instr.get_address();
    
    std::stringstream ss;
    std::string mnemonic = (opcode == Opcode::J) ? "j" : "jal";
    
    // Format: j address
    ss << mnemonic << " 0x" << std::hex << addr;
    
    return ss.str();
}

std::string Decoder::decode(word_t instruction) {
  Instruction instr(instruction);

  switch (instr.get_format()) {
    case InstructionFormat::R_TYPE:
      return decode_r_type(instr);
    case InstructionFormat::I_TYPE:
      return decode_i_type(instr);
    case InstructionFormat::J_TYPE:
      return decode_j_type(instr);
  }

  return "Unknown";
}

Decoder::InstructionDetails Decoder::get_details(word_t instruction) {
  Instruction instr(instruction);
  InstructionDetails details;

  details.mnemonic = decode(instruction);
  details.format = instr.get_format();

  switch (details.format) {
    case InstructionFormat::R_TYPE:
      details.fields.push_back("opcode: " + std::to_string(instr.get_opcode()));
      details.fields.push_back("rs: " + std::to_string(instr.get_rs()));
      details.fields.push_back("rt: " + std::to_string(instr.get_rt()));
      details.fields.push_back("rd: " + std::to_string(instr.get_rd()));
      details.fields.push_back("shamt: " + std::to_string(instr.get_shamt()));
      details.fields.push_back("funct: 0x" + std::to_string(instr.get_funct()));
      details.description = "R-type: Register arithmetic/logic operation";
      break;

    case InstructionFormat::I_TYPE:
      details.fields.push_back("opcode: " + std::to_string(instr.get_opcode()));
      details.fields.push_back("rs: " + std::to_string(instr.get_rs()));
      details.fields.push_back("rt: " + std::to_string(instr.get_rt()));
      details.fields.push_back("immediate: 0x" + std::to_string(instr.get_immediate()));
      details.description = "I-type: Immediate arithmetic/load/store/branch";
      break;

    case InstructionFormat::J_TYPE:
      details.fields.push_back("opcode: " + std::to_string(instr.get_opcode()));
      details.fields.push_back("address: " + std::to_string(instr.get_address()));
      details.description = "J-type: Jump to address";
      break;
  }

  return details;
}

word_t Decoder::assemble(const std::string& assembly_line) {
    return 0;  // TODO: Implement later
}

} // namespace ez_arch
