#include "core/decoder.hpp"
#include "core/instruction.hpp"
#include "core/register_names.hpp"
#include "core/types.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <stdexcept>

namespace ez_arch {

// Decoding functions
static std::string decodeRType(const Instruction& instruction) {
  uint8_t funct = instruction.get_funct();
  uint8_t rs = instruction.get_rs();
  uint8_t rt = instruction.get_rt();
  uint8_t rd = instruction.get_rd();

  std::string mnemonic;

  switch (funct) {
    case Funct::kADD: mnemonic = "add"; break;
    case Funct::kSUB: mnemonic = "sub"; break;
    case Funct::kAND: mnemonic = "and"; break;
    case Funct::kOR: mnemonic = "or"; break;
    case Funct::kSLT: mnemonic = "slt"; break;
    default: return "unknown";
  }

  std::stringstream ss;
  ss << mnemonic << " " << kREGISTER_NAMES[rd] << ", "
      << kREGISTER_NAMES[rs] << ", " << kREGISTER_NAMES[rt];

  return ss.str();
}

static std::string decodeIType(const Instruction& instruction) {
  uint8_t opcode = instruction.get_opcode();
  uint8_t rs = instruction.get_rs();
  uint8_t rt = instruction.get_rt();
  int16_t imm = instruction.get_immediate();

  std::string mnemonic;

  switch (opcode) {
    case Opcode::kADDI: mnemonic = "addi"; break;
    case Opcode::kANDI: mnemonic = "andi"; break;
    case Opcode::kORI: mnemonic = "ori"; break;
    case Opcode::kLW: mnemonic = "lw"; break;
    case Opcode::kSW: mnemonic = "sw"; break;
    case Opcode::kBEQ: mnemonic = "beq"; break;
    case Opcode::kBNE: mnemonic = "bne"; break;
    default: return "unknown";
  }

  std::stringstream ss;

  switch (opcode) {
    case Opcode::kSW:
    case Opcode::kLW:
      ss << mnemonic << " " << kREGISTER_NAMES[rt] << ", " 
          << imm << "(" << kREGISTER_NAMES[rs] << ")";
      break;

    case Opcode::kBEQ:
    case Opcode::kBNE:
      ss << mnemonic << " " << kREGISTER_NAMES[rs] << ", " 
           << kREGISTER_NAMES[rt] << ", " << imm;
      break;

    default:
      ss << mnemonic << " " << kREGISTER_NAMES[rt] << ", " 
        << kREGISTER_NAMES[rs] << ", " << imm;
      break;
  }

  return ss.str();
}

static std::string decodeJType(const Instruction& instruction) {
    uint8_t opcode = instruction.get_opcode();
    uint32_t addr = instruction.get_address();
    
    std::stringstream ss;
    std::string mnemonic = (opcode == Opcode::kJ) ? "j" : "jal";
    
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

Decoder::InstructionDetails Decoder::getDetails(word_t instruction) {
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

// Assembly helper functions
static std::string trim(const std::string& str) {
  size_t start = str.find_first_not_of(" \t\r\n");
  if (start == std::string::npos) { return "";
}
  size_t end = str.find_last_not_of(" \t\r\n");
  return str.substr(start, end - start + 1);
}

static std::vector<std::string> split(const std::string& str, const std::string& delims) {
  std::vector<std::string> tokens;
  size_t start = 0;
  while (start < str.length()) {
    while (start < str.length() && delims.find(str[start]) != std::string::npos) {
      start++;
    }
    if (start >= str.length()) { break;
}
    
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

static uint8_t parseRegister(const std::string& regStr) {
  std::string reg = trim(regStr);
  
  if (reg[0] == '$') {
    if (reg.length() > 1 && (std::isdigit(reg[1]) != 0)) {
      int num = std::stoi(reg.substr(1));
      if (num >= 0 && num < 32) { return static_cast<uint8_t>(num);
}
    }
    
    for (size_t i = 0; i < kREGISTER_NAMES.size(); ++i) {
      if (reg == kREGISTER_NAMES[i]) {
        return static_cast<uint8_t>(i);
      }
    }
  }
  
  throw std::invalid_argument("Invalid register: " + reg);
}

static int32_t parseImmediate(const std::string& immStr) {
  std::string imm = trim(immStr);
  
  if (imm.empty()) {
    throw std::invalid_argument("Empty immediate value");
  }
  
  if (imm.length() >= 2 && imm[0] == '0' && (imm[1] == 'x' || imm[1] == 'X')) {
    return std::stoi(imm, nullptr, 16);
  }
  
  return std::stoi(imm);
}

// Structs for assembly
struct ParsedOperands {
  std::vector<std::string> parts;
  
  [[nodiscard]] size_t count() const { return parts.size(); }
  std::string& operator[](size_t idx) { return parts[idx]; }
  const std::string& operator[](size_t idx) const { return parts[idx]; }
};

struct AssembledInstruction {
  uint8_t opcode = 0;
  uint8_t rs = 0;
  uint8_t rt = 0;
  uint8_t rd = 0;
  int16_t immediate = 0;
  uint32_t address = 0;
  uint8_t funct = 0;
  
  [[nodiscard]] word_t encodeRType() const {
    return (opcode << 26) | (rs << 21) | (rt << 16) | 
           (rd << 11) | (0 << 6) | funct;
  }
  
  [[nodiscard]] word_t encodeIType() const {
    return (opcode << 26) | (rs << 21) | (rt << 16) | 
           (immediate & 0xFFFF);
  }
  
  [[nodiscard]] word_t encodeJType() const {
    return (opcode << 26) | (address & 0x3FFFFFF);
  }
};

static ParsedOperands parseOperands(const std::string& operandsStr) {
  ParsedOperands result;
  std::vector<std::string> tokens = split(operandsStr, ", ()");
  
  for (const auto& token : tokens) {
    if (!token.empty()) {
      result.parts.push_back(token);
    }
  }
  
  return result;
}

static void validateOperandCount(const ParsedOperands& ops, size_t expected, 
                            const std::string& instructionName) {
  if (ops.count() != expected) {
    throw std::invalid_argument(instructionName + " requires " + 
                                std::to_string(expected) + " operands, got " + 
                                std::to_string(ops.count()));
  }
}

static AssembledInstruction assembleRType(const ParsedOperands& ops, 
                                      const std::string& mnemonic) {
  validateOperandCount(ops, 3, mnemonic);
  
  AssembledInstruction instruction;
  instruction.rd = parseRegister(ops[0]);
  instruction.rs = parseRegister(ops[1]);
  instruction.rt = parseRegister(ops[2]);
  instruction.opcode = 0;
  
  if (mnemonic == "add") { instruction.funct = Funct::kADD;
  } else if (mnemonic == "sub") { instruction.funct = Funct::kSUB;
  } else if (mnemonic == "and") { instruction.funct = Funct::kAND;
  } else if (mnemonic == "or") { instruction.funct = Funct::kOR;
  } else if (mnemonic == "slt") { instruction.funct = Funct::kSLT;
  } else { throw std::invalid_argument("Unknown R-type instruction: " + mnemonic);
}
  
  return instruction;
}

static AssembledInstruction assembleImmediateArith(const ParsedOperands& ops,
                                               const std::string& mnemonic) {
  validateOperandCount(ops, 3, mnemonic);
  
  AssembledInstruction instruction;
  instruction.rt = parseRegister(ops[0]);
  instruction.rs = parseRegister(ops[1]);
  instruction.immediate = static_cast<int16_t>(parseImmediate(ops[2]));
  
  if (mnemonic == "addi") { instruction.opcode = Opcode::kADDI;
  } else if (mnemonic == "andi") { instruction.opcode = Opcode::kANDI;
  } else if (mnemonic == "ori") { instruction.opcode = Opcode::kORI;
  } else { throw std::invalid_argument("Unknown immediate arithmetic: " + mnemonic);
}
  
  return instruction;
}

static std::pair<int16_t, uint8_t> parseMemoryReference(const std::string& memRef) {
  size_t parenStart = memRef.find('(');
  size_t parenEnd = memRef.find(')');
  
  if (parenStart == std::string::npos || parenEnd == std::string::npos) {
    throw std::invalid_argument("Invalid memory reference format: " + memRef);
  }
  
  std::string offsetStr = trim(memRef.substr(0, parenStart));
  std::string baseStr = trim(memRef.substr(parenStart + 1, 
                                             parenEnd - parenStart - 1));
  
  auto offset = static_cast<int16_t>(parseImmediate(offsetStr));
  uint8_t base = parseRegister(baseStr);
  
  return {offset, base};
}

static AssembledInstruction assembleLoadStore(const ParsedOperands& ops,
                                          const std::string& mnemonic) {
  if (ops.count() != 2) {
    throw std::invalid_argument(mnemonic + " requires 2 operands");
  }
  
  AssembledInstruction instruction;
  instruction.rt = parseRegister(ops[0]);
  
  auto [offset, base] = parseMemoryReference(ops[1]);
  instruction.immediate = offset;
  instruction.rs = base;
  
  if (mnemonic == "lw") { instruction.opcode = Opcode::kLW;
  } else if (mnemonic == "sw") { instruction.opcode = Opcode::kSW;
  } else { throw std::invalid_argument("Unknown load/store: " + mnemonic);
}
  
  return instruction;
}

static AssembledInstruction assembleBranch(const ParsedOperands& ops,
                                            const std::string& mnemonic) {
  validateOperandCount(ops, 3, mnemonic);
  
  AssembledInstruction instruction;
  instruction.rs = parseRegister(ops[0]);
  instruction.rt = parseRegister(ops[1]);
  instruction.immediate = static_cast<int16_t>(parseImmediate(ops[2]));
  
  if (mnemonic == "beq") { instruction.opcode = Opcode::kBEQ;
  } else if (mnemonic == "bne") { instruction.opcode = Opcode::kBNE;
  } else { throw std::invalid_argument("Unknown branch instruction: " + mnemonic);
}
  
  return instruction;
}

static AssembledInstruction assembleJump(const ParsedOperands& ops,
                                          const std::string& mnemonic) {
  validateOperandCount(ops, 1, mnemonic);
  
  AssembledInstruction instruction;
  uint32_t addr = parseImmediate(trim(ops[0]));
  
  if (addr >= (1U << 26)) {
    throw std::invalid_argument("Jump address too large (max 26 bits)");
  }
  
  instruction.address = addr;
  
  if (mnemonic == "j") { instruction.opcode = Opcode::kJ;
  } else if (mnemonic == "jal") { instruction.opcode = Opcode::kJAL;
  } else { throw std::invalid_argument("Unknown jump instruction: " + mnemonic);
}
  
  return instruction;
}

static AssembledInstruction assembleByType(const std::string& mnemonic,
                                             const ParsedOperands& ops) {
  if (mnemonic == "add" || mnemonic == "sub" || mnemonic == "and" || 
      mnemonic == "or" || mnemonic == "slt") {
    return assembleRType(ops, mnemonic);
  }
  
  if (mnemonic == "addi" || mnemonic == "andi" || mnemonic == "ori") {
    return assembleImmediateArith(ops, mnemonic);
  }
  
  if (mnemonic == "lw" || mnemonic == "sw") {
    return assembleLoadStore(ops, mnemonic);
  }
  
  if (mnemonic == "beq" || mnemonic == "bne") {
    return assembleBranch(ops, mnemonic);
  }
  
  if (mnemonic == "j" || mnemonic == "jal") {
    return assembleJump(ops, mnemonic);
  }
  
  throw std::invalid_argument("Unknown instruction: " + mnemonic);
}

word_t Decoder::assemble(const std::string& assemblyLine) {
  std::string line = trim(assemblyLine);
  
  if (line.empty() || line[0] == '#') {
    return 0;
  }
  
  size_t spacePos = line.find(' ');
  std::string mnemonic = (spacePos != std::string::npos) 
    ? line.substr(0, spacePos) : line;
  std::transform(mnemonic.begin(), mnemonic.end(), mnemonic.begin(), ::tolower);
  
  std::string operandsStr = (spacePos != std::string::npos)
    ? line.substr(spacePos + 1) : "";
  ParsedOperands ops = parseOperands(operandsStr);
  
  AssembledInstruction instruction = assembleByType(mnemonic, ops);
  
  if (mnemonic == "add" || mnemonic == "sub" || mnemonic == "and" || 
      mnemonic == "or" || mnemonic == "slt") {
    return instruction.encode_r_type();
  }
  
  if (mnemonic == "j" || mnemonic == "jal") {
    return instruction.encode_j_type();
  }
  
  return instruction.encode_i_type();
}

} // namespace ez_arch
