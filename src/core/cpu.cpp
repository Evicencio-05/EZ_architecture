#include "core/cpu.hpp"
#include "core/alu.hpp"
#include <string>
#include <string_view>
#include <iostream>

namespace ez_arch {

CPU::CPU() : current_instruction_(0), current_stage_(ExecutionStage::FETCH), halted_(false) {
  pipeline_.clear();
}

void CPU::clear_pipeline() {
  pipeline_.clear();
}

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
  clear_pipeline();
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
  
  std::string_view control_bits;
  
  switch (opcode) {
    case 0x00:
      control_bits = "1000010001";
      break;

    case Opcode::LW:
      control_bits = "0001100011";
      break;

    case Opcode::SW:
      control_bits = "0000000110";
      break;

    case Opcode::BEQ: [[fallthrough]];
    case Opcode::BNE:
      control_bits = "0010001000";  // ALUOp=01 for SUB
      break;

    case Opcode::ADDI:
      control_bits = "0000000011";
      break;

    case Opcode::ANDI:
      control_bits = "0000010011";
      break;

    case Opcode::ORI:
      control_bits = "0000010011";
      break;

    case Opcode::J: [[fallthrough]];
    case Opcode::JAL:
      control_bits = "0100000001";
      break;

    default:
      ctrl.clear();
      return ctrl;
  }

  ctrl.RegDst = (control_bits[0] == '1');
  ctrl.Jump = (control_bits[1] == '1');
  ctrl.Branch = (control_bits[2] == '1');
  ctrl.MemRead = (control_bits[3] == '1');
  ctrl.MemToReg = (control_bits[4] == '1');
  std::string aluop_str(control_bits.substr(5, 2));
  ctrl.ALUOp = std::stoi(aluop_str, nullptr, 2);
  ctrl.MemWrite = (control_bits[7] == '1');
  ctrl.ALUSrc = (control_bits[8] == '1');
  ctrl.RegWrite = (control_bits[9] == '1');

  return ctrl;
}

ALUOperation CPU::alu_control(uint8_t ALUOp, uint8_t funct) {
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
  word_t alu_input_2;
  uint8_t opcode = current_instruction_.get_opcode();
  ALUOperation alu_op;
  
  if (pipeline_.control.ALUSrc) {
    // Use sign-extended immediate
    alu_input_2 = static_cast<word_t>(static_cast<int32_t>(pipeline_.immediate));

    if (opcode == Opcode::ANDI || opcode == Opcode::ORI) {
      alu_op = (opcode == Opcode::ANDI) ? ALUOperation::AND : ALUOperation::OR;
      alu_input_2 = static_cast<word_t>(pipeline_.immediate) & 0xFFFF;
    } else {
      alu_op = alu_control(pipeline_.control.ALUOp, pipeline_.funct);
    }
  } else {
    // Use register value
    alu_input_2 = pipeline_.read_data_2;
    alu_op = alu_control(pipeline_.control.ALUOp, pipeline_.funct);
  }
  
  // Execute ALU operation
  ALU::Result result = ALU::execute(alu_op, pipeline_.read_data_1, alu_input_2);
  pipeline_.alu_result = result.value;
  pipeline_.zero_flag = result.zero;

  pipeline_.write_reg = pipeline_.control.RegDst ? pipeline_.rd : pipeline_.rt;
  pipeline_.mem_write_data = pipeline_.read_data_2;
  
  word_t pc = registers_.get_pc();

  if (pipeline_.control.Branch) {
    bool take_branch = false;

    if (opcode == Opcode::BEQ) {
      take_branch = pipeline_.zero_flag;
    } else if (opcode == Opcode::BNE) {
      take_branch = !pipeline_.zero_flag;
    }

    if (take_branch) {
      word_t sign_extended_imm = static_cast<word_t>(static_cast<int32_t>(pipeline_.immediate));
      registers_.set_pc(pc + (sign_extended_imm << 2));
    }
  }

  if (pipeline_.control.Jump) {
    address_t addr = current_instruction_.get_address();
    word_t jump_addr = ((pc + 4) & 0xF0000000) | (addr << 2);

    if (opcode == Opcode::JAL) {
      pipeline_.wb_data = pc + 4;
      pipeline_.write_reg = 31; // $ra
    }

    registers_.set_pc(jump_addr - 4);
  }

}

void CPU::memory_access() {
  if (pipeline_.control.MemRead) {
    pipeline_.mem_read_data = memory_.read_word(pipeline_.alu_result);
  }

  if (pipeline_.control.MemWrite) {
    memory_.write_word(pipeline_.alu_result, pipeline_.mem_write_data);
  }
}

void CPU::write_back() {
  if (pipeline_.control.RegWrite) {
    word_t write_data;
    if (pipeline_.control.MemToReg) {
      write_data = pipeline_.mem_read_data;
    } else if (pipeline_.control.Jump && current_instruction_.get_opcode() == Opcode::JAL) {
      write_data = pipeline_.wb_data;
    } else {
      write_data = pipeline_.alu_result;
    }

    registers_.write(pipeline_.write_reg, write_data);
  }

  registers_.increment_pc();

  pipeline_.clear();
}
} // namespace ez_arch
