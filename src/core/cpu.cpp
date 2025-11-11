#include "core/cpu.hpp"
#include "core/alu.hpp"
#include <string>
#include <string_view>
#include <iostream>

namespace ez_arch {

CPU::CPU() : m_currentInstruction(0), m_currentStage(ExecutionStage::FETCH), m_halted(false) {
  m_pipeline.clear();
}

void CPU::clear_pipeline() {
  m_pipeline.clear();
}

void CPU::load_program(const std::vector<word_t>& program) {
  m_memory.load_program(program, 0);
  m_registers.set_pc(0);
  m_halted = false;
}

void CPU::step() {
  if (m_halted) return;
  
  // Ensure we start from FETCH stage
  while (m_currentStage != ExecutionStage::FETCH) {
    step_stage();
  }

  fetch();
  
  // Check halt condition after fetch
  if (m_currentInstruction.get_raw() == 0) {
    m_halted = true;
    return;
  }
  
  decode();
  execute();
  m_memoryaccess();
  write_back();
}

void CPU::run() {
  while(!m_halted) {
    step();
  }
}

void CPU::reset() {
  m_registers.reset();
  m_memory.reset();
  m_currentStage = ExecutionStage::FETCH;
  m_halted = false;
  clear_pipeline();
}

void CPU::step_stage() {
  if (m_halted) return;

  if (m_stageCallback) m_stageCallback(m_currentStage);

  switch (m_currentStage) {
    case ExecutionStage::FETCH:
      fetch();
      m_currentStage = ExecutionStage::DECODE;
      break;
    case ExecutionStage::DECODE:
      decode();
      m_currentStage = ExecutionStage::EXECUTE;
      break;
    case ExecutionStage::EXECUTE:
      execute();
      m_currentStage = ExecutionStage::MEMORY_ACCESS;
      break;
    case ExecutionStage::MEMORY_ACCESS:
      m_memoryaccess();
      m_currentStage = ExecutionStage::WRITE_BACK;
      break;
    case ExecutionStage::WRITE_BACK:
      write_back();
      m_currentStage = ExecutionStage::FETCH;
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
  word_t pc = m_registers.get_pc();
  word_t instruction_word = m_memory.read_word(pc);
  m_currentInstruction = Instruction(instruction_word);
}

void CPU::decode() {
  m_pipeline.rs = m_currentInstruction.get_rs();
  m_pipeline.rt = m_currentInstruction.get_rt();
  m_pipeline.rd = m_currentInstruction.get_rd();
  m_pipeline.funct = m_currentInstruction.get_funct();
  m_pipeline.immediate = m_currentInstruction.get_immediate();

  m_pipeline.read_data_1 = m_registers.read(m_pipeline.rs);
  m_pipeline.read_data_2 = m_registers.read(m_pipeline.rt);

  m_pipeline.control = generate_control_signals(m_currentInstruction.get_opcode());
}

void CPU::execute() {
  word_t alu_input_2;
  uint8_t opcode = m_currentInstruction.get_opcode();
  ALUOperation alu_op;
  
  if (m_pipeline.control.ALUSrc) {
    // Use sign-extended immediate
    alu_input_2 = static_cast<word_t>(static_cast<int32_t>(m_pipeline.immediate));

    if (opcode == Opcode::ANDI || opcode == Opcode::ORI) {
      alu_op = (opcode == Opcode::ANDI) ? ALUOperation::AND : ALUOperation::OR;
      alu_input_2 = static_cast<word_t>(m_pipeline.immediate) & 0xFFFF;
    } else {
      alu_op = alu_control(m_pipeline.control.ALUOp, m_pipeline.funct);
    }
  } else {
    // Use register value
    alu_input_2 = m_pipeline.read_data_2;
    alu_op = alu_control(m_pipeline.control.ALUOp, m_pipeline.funct);
  }
  
  // Execute ALU operation
  ALU::Result result = ALU::execute(alu_op, m_pipeline.read_data_1, alu_input_2);
  m_pipeline.alu_result = result.value;
  m_pipeline.zero_flag = result.zero;

  m_pipeline.write_reg = m_pipeline.control.RegDst ? m_pipeline.rd : m_pipeline.rt;
  m_pipeline.mem_write_data = m_pipeline.read_data_2;
  
  word_t pc = m_registers.get_pc();

  if (m_pipeline.control.Branch) {
    bool take_branch = false;

    if (opcode == Opcode::BEQ) {
      take_branch = m_pipeline.zero_flag;
    } else if (opcode == Opcode::BNE) {
      take_branch = !m_pipeline.zero_flag;
    }

    if (take_branch) {
      word_t sign_extended_imm = static_cast<word_t>(static_cast<int32_t>(m_pipeline.immediate));
      m_registers.set_pc(pc + (sign_extended_imm << 2));
    }
  }

  if (m_pipeline.control.Jump) {
    address_t addr = m_currentInstruction.get_address();
    word_t jump_addr = ((pc + 4) & 0xF0000000) | (addr << 2);

    if (opcode == Opcode::JAL) {
      m_pipeline.wb_data = pc + 4;
      m_pipeline.write_reg = 31; // $ra
    }

    m_registers.set_pc(jump_addr - 4);
  }

}

void CPU::m_memoryaccess() {
  if (m_pipeline.control.MemRead) {
    m_pipeline.mem_read_data = m_memory.read_word(m_pipeline.alu_result);
  }

  if (m_pipeline.control.MemWrite) {
    m_memory.write_word(m_pipeline.alu_result, m_pipeline.mem_write_data);
  }
}

void CPU::write_back() {
  if (m_pipeline.control.RegWrite) {
    word_t write_data;
    if (m_pipeline.control.MemToReg) {
      write_data = m_pipeline.mem_read_data;
    } else if (m_pipeline.control.Jump && m_currentInstruction.get_opcode() == Opcode::JAL) {
      write_data = m_pipeline.wb_data;
    } else {
      write_data = m_pipeline.alu_result;
    }

    m_registers.write(m_pipeline.write_reg, write_data);
  }

  m_registers.increment_pc();

  m_pipeline.clear();
}
} // namespace ez_arch
