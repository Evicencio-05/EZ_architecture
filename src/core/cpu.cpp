#include "core/cpu.hpp"
#include "core/alu.hpp"
#include <string>
#include <string_view>

namespace ez_arch {

CPU::CPU() : current_instruction_(0), current_stage_(ExecutionStage::FETCH), halted_(false) {}

void CPU::load_program(const std::vector<word_t>& program) {
  memory_.load_program(program, 0);
  registers_.set_pc(0);
  halted_ = false;
}

void CPU::step() {
  if (halted_) return;
  
  // Ensure we start from FETCH stage
  while (current_stage_ != ExecutionStage::FETCH) {
    step_stage();
  }

  fetch();
  
  // Check halt condition after fetch
  if (current_instruction_.get_raw() == 0) {
    halted_ = true;
    return;
  }
  
  decode();
  execute();
  memory_access();
  write_back();
}

void CPU::run() {
  while(!halted_) {
    step();
  }
}

void CPU::reset() {
  registers_.reset();
  memory_.reset();
  current_stage_ = ExecutionStage::FETCH;
  halted_ = false;
}

void CPU::step_stage() {
  if (halted_) return;

  if (stage_callback_) stage_callback_(current_stage_);

  switch (current_stage_) {
    case ExecutionStage::FETCH:
      fetch();
      current_stage_ = ExecutionStage::DECODE;
      break;
    case ExecutionStage::DECODE:
      decode();
      current_stage_ = ExecutionStage::EXECUTE;
      break;
    case ExecutionStage::EXECUTE:
      execute();
      current_stage_ = ExecutionStage::MEMORY_ACCESS;
      break;
    case ExecutionStage::MEMORY_ACCESS:
      memory_access();
      current_stage_ = ExecutionStage::WRITE_BACK;
      break;
    case ExecutionStage::WRITE_BACK:
      write_back();
      current_stage_ = ExecutionStage::FETCH;
      break;
  }
}

ControlSignals CPU::generate_control_signals(uint8_t opcode) {
  ControlSignals ctrl;
  ctrl.clear();
  
  switch (opcode) {
    case 0x00:
      std::string_view control_bits = "1000010001"
      break;

    case Opcode::LW:
      std::string_view control_bits = "0001100011"
      break;

    case Opcode::SW:
      std::string_view control_bits = "0000000110"
      break;

    case Opcode::BEQ: [[fallthrough]]
    case Opcode::BNE:
      std::string_view control_bits = "0010001000"
      break;

    case Opcode::ADDI:
      std::string_view control_bits = "0000000011"
      break;

    case Opcode::ANDI:
      std::string_view control_bits = "0000010011"
      break;

    case Opcode::ORI:
      std::string_view control_bits = "0000010011"
      break;

    case Opcode::J: [[fallthrough]];
    case Opcode::JAL:
      std::string_view control_bits = "0100000001"
      break;

    default:
      ctrl.clear();
      break;

  }

  ctrl.RegDst static_cast<bool>(control_bits[0]);
  ctrl.Jump static_cast<bool>(control_bits[1]);
  ctrl.Branch static_cast<bool>(control_bits[2]);
  ctrl.MemRead static_cast<bool>(control_bits[3]);
  ctrl.MemToReg static_cast<bool>(control_bits[4]);
  ctrl.ALUOp = static_cast<uint8_t>(control_bits.substr(5,2));
  ctrl.MemWrite static_cast<bool>(control_bits[7]);
  ctrl.ALUSrc static_cast<bool>(control_bits[8]);
  ctrl.RegWrite static_cast<bool>(control_bits[9]);

  return ctrl;
}

ALUOperation CPU::alu_control(uint8_t ALUOp, uint8_t, funct) {
  if (ALUOp == 0b00) {
    return ALUOperation::ADD;
  } else if (ALUOp == 0b01) {
    return ALUOperation::SUB;
  } else if (ALUOp == 0b10) {
    switch (funct) {
      case Funct::ADD: return ALUOperation::ADD;
      case Funct::SUB: return ALUOperation::SUB;
      case Funct::AND: return ALUOperation::AND;
      case Funct::OR: return ALUOperation::OR;
      case Funct::SLT: return ALUOperation::SLT;
      default: return ALUOperation::ADD;
    }
  } else if (ALUOp == 0b11) {
    return ALUOperation::ADD; // placeholder
  }

  return ALUOperation::ADD;
}

// Pipeline stages
void CPU::fetch() {
  word_t pc = registers_.get_pc();
  word_t instruction_word = memory_.read_word(pc);
  current_instruction_ = Instruction(instruction_word);
}

void CPU::decode() {
  pipeline_.rs = current_instruction_.get_rs();
  pipeline_.rt = current_instruction_.get_rt();
  pipeline_.rd = current_instruction_.get_rd();
  pipeline_.funct = current_instruction_.get_funct();
  pipeline_.immediate = current_instruction_.get_immediate();

  pipeline_.read_data_1 = registers_.read(pipeline_.rs);
  pipeline_.read_data_2 = registers_.read(pipeline_.rt);

  pipeline_.control = generate_control_signals(current_instruction_.get_opcode());
}

void CPU::execute() {
  // NOTE: write_back() always increments PC by 4
  // Branches/jumps compensate by setting PC relative to their target

  switch (current_instruction_.get_format()) {
    case InstructionFormat::R_TYPE:
      execute_r_type(current_instruction_);
      break;
    case InstructionFormat::I_TYPE:
      execute_i_type(current_instruction_);
      break;
    case InstructionFormat::J_TYPE:
      execute_j_type(current_instruction_);
      break;
  }
}

void CPU::memory_access() {
    // TODO Empty for now
}

void CPU::write_back() {
  // Results are written in execute stage for simplicity
  // In a real pipeline, you'd defer writes to this stage

  registers_.increment_pc();
}

// Instruction execution
void CPU::execute_r_type(const Instruction& instr) {
  uint8_t rs = instr.get_rs();
  uint8_t rt = instr.get_rt();
  uint8_t rd = instr.get_rd();
  uint8_t funct = instr.get_funct();

  word_t rs_value = registers_.read(rs);
  word_t rt_value = registers_.read(rt);

  ALU::Result result;

  switch (funct) {
    case Funct::ADD:
      result = ALU::execute(ALUOperation::ADD, rs_value, rt_value);
      break;
    case Funct::SUB:
      result = ALU::execute(ALUOperation::SUB, rs_value, rt_value);
      break;
    case Funct::AND:
      result = ALU::execute(ALUOperation::AND, rs_value, rt_value);
      break;
    case Funct::OR:
      result = ALU::execute(ALUOperation::OR, rs_value, rt_value);
      break;
    case Funct::SLT:
      result = ALU::execute(ALUOperation::SLT, rs_value, rt_value);
      break;
    default:
      return;
  }

  registers_.write(rd, result.value);
}

void CPU::execute_i_type(const Instruction& instr) {
  uint8_t opcode = instr.get_opcode();
  uint8_t rs = instr.get_rs();
  uint8_t rt = instr.get_rt();
  int16_t imm = instr.get_immediate();

  word_t rs_value = registers_.read(rs);
  word_t sign_extended_imm = static_cast<word_t>(static_cast<int32_t>(imm));

  switch (opcode) {
    case Opcode::ADDI: {
      ALU::Result result = ALU::execute(ALUOperation::ADD, rs_value, sign_extended_imm);
      registers_.write(rt, result.value);
      break;
    }

    case Opcode::ANDI: {
      ALU::Result result = ALU::execute(ALUOperation::AND, rs_value, static_cast<word_t>(imm) & 0xFFFF);
      registers_.write(rt, result.value);
      break;
    }

    case Opcode::ORI: {
      ALU::Result result = ALU::execute(ALUOperation::OR, rs_value, static_cast<word_t>(imm) & 0xFFFF);
      registers_.write(rt, result.value);
      break;
    }

    case Opcode::LW: {
      address_t addr = rs_value + sign_extended_imm;
      word_t data = memory_.read_word(addr);
      registers_.write(rt, data);
      break;
    }

    case Opcode::SW: {
      address_t addr = rs_value + sign_extended_imm;
      word_t data = registers_.read(rt);
      memory_.write_word(addr, data);
      break;
    }
    
    case Opcode::BEQ: {
      word_t rt_value = registers_.read(rt);
      if (rs_value == rt_value) {
        word_t pc = registers_.get_pc();
        // Branch target = PC + 4 + (offset << 2)
        // But write_back will add 4, so we set PC to: PC + (offset << 2)
        registers_.set_pc(pc + (sign_extended_imm << 2));
      }
      break;
    }

    case Opcode::BNE: {
      word_t rt_value = registers_.read(rt);
      if (rs_value != rt_value) {
        word_t pc = registers_.get_pc();
        // Branch target = PC + 4 + (offset << 2)
        // But write_back will add 4, so we set PC to: PC + (offset << 2)
        registers_.set_pc(pc + (sign_extended_imm << 2));
      }
      break;
    }

    default:
      break;
  }
}

void CPU::execute_j_type(const Instruction& instr) {
  uint8_t opcode = instr.get_opcode();
  uint32_t addr = instr.get_address();

  word_t pc = registers_.get_pc();
  word_t jump_addr = (pc & 0xF0000000) | (addr << 2);
  register_id_t ra = 31; // $ra at register 31

  if (opcode == Opcode::JAL) {
    registers_.write(ra, pc + 4);
  }

  registers_.set_pc(jump_addr - 4);
}

} // namespace ez_arch
