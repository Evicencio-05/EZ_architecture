#include "core/decoder.hpp"
#include "core/instruction.hpp"
#include "core/register_names.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <stdexcept>

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

// Helper function to trim whitespace
static std::string trim(const std::string& str) {
  size_t start = str.find_first_not_of(" \t\r\n");
  if (start == std::string::npos) return "";
  size_t end = str.find_last_not_of(" \t\r\n");
  return str.substr(start, end - start + 1);
}

// Helper function to split string by delimiters
static std::vector<std::string> split(const std::string& str, const std::string& delims) {
  std::vector<std::string> tokens;
  size_t start = 0;
  while (start < str.length()) {
    // Skip delimiters
    while (start < str.length() && delims.find(str[start]) != std::string::npos) {
      start++;
    }
    if (start >= str.length()) break;
    
    // Find end of token
    size_t end = start;
    while (end < str.length() && delims.find(str[end]) == std::string::npos) {
      end++;
    }
    
    if (end > start) {
      tokens.push_back(str.substr(start, end - start));
    }
    start = end;
  }
  return tokens;
}

// Parse register name to number
static uint8_t parse_register(const std::string& reg_str) {
  std::string reg = trim(reg_str);
  
  // Check for numeric format ($0, $1, etc.)
  if (reg[0] == '$') {
    if (reg.length() > 1 && std::isdigit(reg[1])) {
      int num = std::stoi(reg.substr(1));
      if (num >= 0 && num < 32) return static_cast<uint8_t>(num);
    }
    
    // Check for named registers
    for (size_t i = 0; i < REGISTER_NAMES.size(); ++i) {
      if (reg == REGISTER_NAMES[i]) {
        return static_cast<uint8_t>(i);
      }
    }
  }
  
  throw std::invalid_argument("Invalid register: " + reg);
}

// Parse immediate value
static int32_t parse_immediate(const std::string& imm_str) {
  std::string imm = trim(imm_str);
  
  if (imm.empty()) {
    throw std::invalid_argument("Empty immediate value");
  }
  
  // Hex format: 0x...
  if (imm.length() >= 2 && imm[0] == '0' && (imm[1] == 'x' || imm[1] == 'X')) {
    return std::stoi(imm, nullptr, 16);
  }
  
  // Decimal format
  return std::stoi(imm);
}

word_t Decoder::assemble(const std::string& assembly_line) {
  std::string line = trim(assembly_line);
  
  // Empty or comment line
  if (line.empty() || line[0] == '#') {
    return 0;
  }
  
  // Parse mnemonic
  size_t space_pos = line.find(' ');
  std::string mnemonic = (space_pos != std::string::npos) 
    ? line.substr(0, space_pos) : line;
  std::transform(mnemonic.begin(), mnemonic.end(), mnemonic.begin(), ::tolower);
  
  std::string operands_str = (space_pos != std::string::npos)
    ? line.substr(space_pos + 1) : "";
  
  word_t instruction = 0;
  
  // R-type instructions
  if (mnemonic == "add" || mnemonic == "sub" || mnemonic == "and" || 
      mnemonic == "or" || mnemonic == "slt") {
    
    // Parse: mnemonic $rd, $rs, $rt
    std::vector<std::string> ops = split(operands_str, ", ");
    if (ops.size() != 3) {
      throw std::invalid_argument("R-type requires 3 operands");
    }
    
    uint8_t rd = parse_register(ops[0]);
    uint8_t rs = parse_register(ops[1]);
    uint8_t rt = parse_register(ops[2]);
    
    uint8_t funct = 0;
    if (mnemonic == "add") funct = Funct::ADD;
    else if (mnemonic == "sub") funct = Funct::SUB;
    else if (mnemonic == "and") funct = Funct::AND;
    else if (mnemonic == "or") funct = Funct::OR;
    else if (mnemonic == "slt") funct = Funct::SLT;
    
    // R-type format:
    //             opcode(6)|  rs(5)     |  rt(5)     |  rd(5)     |  shamt(5)| funct(6)
    instruction = (0 << 26) | (rs << 21) | (rt << 16) | (rd << 11) | (0 << 6) | funct;
  }
  // I-type arithmetic instructions
  else if (mnemonic == "addi" || mnemonic == "andi" || mnemonic == "ori") {
    // Parse: mnemonic $rt, $rs, immediate
    std::vector<std::string> ops = split(operands_str, ", ");
    if (ops.size() != 3) {
      throw std::invalid_argument("I-type arithmetic requires 3 operands");
    }
    
    uint8_t rt = parse_register(ops[0]);
    uint8_t rs = parse_register(ops[1]);
    int16_t imm = static_cast<int16_t>(parse_immediate(ops[2]));
    
    uint8_t opcode = 0;
    if (mnemonic == "addi") opcode = Opcode::ADDI;
    else if (mnemonic == "andi") opcode = Opcode::ANDI;
    else if (mnemonic == "ori") opcode = Opcode::ORI;
    
    // I-type format: 
    //             opcode(6)     |  rs(5)     |  rt(5)     |  immediate(16)
    instruction = (opcode << 26) | (rs << 21) | (rt << 16) | (imm & 0xFFFF);
  }
  // Load/Store instructions
  else if (mnemonic == "lw" || mnemonic == "sw") {
    // Parse: lw/sw $rt, offset($rs)
    size_t comma_pos = operands_str.find(',');
    if (comma_pos == std::string::npos) {
      throw std::invalid_argument("Load/Store requires offset($rs) format");
    }
    
    std::string rt_str = trim(operands_str.substr(0, comma_pos));
    std::string offset_base = trim(operands_str.substr(comma_pos + 1));
    
    // Parse offset($rs)
    size_t paren_start = offset_base.find('(');
    size_t paren_end = offset_base.find(')');
    if (paren_start == std::string::npos || paren_end == std::string::npos) {
      throw std::invalid_argument("Load/Store requires offset($rs) format");
    }
    
    std::string offset_str = trim(offset_base.substr(0, paren_start));
    std::string rs_str = trim(offset_base.substr(paren_start + 1, paren_end - paren_start - 1));
    
    uint8_t rt = parse_register(rt_str);
    uint8_t rs = parse_register(rs_str);
    int16_t offset = static_cast<int16_t>(parse_immediate(offset_str));
    
    uint8_t opcode = (mnemonic == "lw") ? Opcode::LW : Opcode::SW;
    
    // I-type format: 
    //             opcode(6)     |  rs(5)     |  rt(5)     |  immediate(16)
    instruction = (opcode << 26) | (rs << 21) | (rt << 16) | (offset & 0xFFFF);
  }
  // Branch instructions
  else if (mnemonic == "beq" || mnemonic == "bne") {
    // Parse: beq/bne $rs, $rt, offset
    std::vector<std::string> ops = split(operands_str, ", ");
    if (ops.size() != 3) {
      throw std::invalid_argument("Branch requires 3 operands");
    }
    
    uint8_t rs = parse_register(ops[0]);
    uint8_t rt = parse_register(ops[1]);
    int16_t offset = static_cast<int16_t>(parse_immediate(ops[2]));
    
    uint8_t opcode = (mnemonic == "beq") ? Opcode::BEQ : Opcode::BNE;
    
    // I-type format: 
    //             opcode(6)     |  rs(5)     |  rt(5)     |  immediate(16)
    instruction = (opcode << 26) | (rs << 21) | (rt << 16) | (offset & 0xFFFF);
  }
  // Jump instructions
  else if (mnemonic == "j" || mnemonic == "jal") {
    // Parse: j/jal address
    uint32_t addr = parse_immediate(trim(operands_str));
    
    // Address should be 26 bits
    if (addr >= (1 << 26)) {
      throw std::invalid_argument("Jump address too large (max 26 bits)");
    }
    
    uint8_t opcode = (mnemonic == "j") ? Opcode::J : Opcode::JAL;
    
    // J-type format: 
    //             opcode(6)     |  address(26)
    instruction = (opcode << 26) | (addr & 0x3FFFFFF);
  }
  else {
    throw std::invalid_argument("Unknown instruction: " + mnemonic);
  }
  
  return instruction;
}

} // namespace ez_arch
