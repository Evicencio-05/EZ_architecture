#include "core/cpu.hpp"
#include "core/alu.hpp"
#include <string>
#include <string_view>

namespace ez_arch {

CPU::CPU() : m_currentInstruction(0) {
  m_pipeline.clear();
}

void CPU::clearPipeline() {
  m_pipeline.clear();
}

void CPU::loadProgram(const std::vector<word_t>& program) {
  m_memory.loadProgram(program, 0);
  m_registers.setPc(0);
  m_halted = false;
}

void CPU::step() {
  if (m_halted) { return;
}
  
  // Ensure we start from FETCH stage
  while (m_currentStage != ExecutionStage::FETCH) {
    stepStage();
  }

  fetch();
  
  // Check halt condition after fetch
  if (m_currentInstruction.getRaw() == 0) {
    m_halted = true;
    return;
  }
  
  decode();
  execute();
  mMemoryaccess();
  writeBack();
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
  clearPipeline();
}

void CPU::stepStage() {
  if (m_halted) { return;
}

  if (m_stageCallback) { m_stageCallback(m_currentStage);
}

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
      mMemoryaccess();
      m_currentStage = ExecutionStage::WRITE_BACK;
      break;
    case ExecutionStage::WRITE_BACK:
      writeBack();
      m_currentStage = ExecutionStage::FETCH;
      break;
  }
}

ControlSignals CPU::generateControlSignals(uint8_t opcode) {
  ControlSignals ctrl;
  ctrl.clear();
  
  std::string_view controlBits;
  
  switch (opcode) {
    case 0x00:
      controlBits = "1000010001";
      break;

    case Opcode::kLW:
      controlBits = "0001100011";
      break;

    case Opcode::kSW:
      controlBits = "0000000110";
      break;

    case Opcode::kBEQ: [[fallthrough]];
    case Opcode::kBNE:
      controlBits = "0010001000";  // ALUOp=01 for SUB
      break;

    case Opcode::kADDI:
      controlBits = "0000000011";
      break;

    case Opcode::kANDI:
      controlBits = "0000010011";
      break;

    case Opcode::kORI:
      controlBits = "0000010011";
      break;

    case Opcode::kJ: [[fallthrough]];
    case Opcode::kJAL:
      controlBits = "0100000001";
      break;

    default:
      ctrl.clear();
      return ctrl;
  }

  ctrl.RegDst = (controlBits[0] == '1');
  ctrl.Jump = (controlBits[1] == '1');
  ctrl.Branch = (controlBits[2] == '1');
  ctrl.MemRead = (controlBits[3] == '1');
  ctrl.MemToReg = (controlBits[4] == '1');
  std::string aluopStr(controlBits.substr(5, 2));
  ctrl.ALUOp = std::stoi(aluopStr, nullptr, 2);
  ctrl.MemWrite = (controlBits[7] == '1');
  ctrl.ALUSrc = (controlBits[8] == '1');
  ctrl.RegWrite = (controlBits[9] == '1');

  return ctrl;
}

ALUOperation CPU::aluControl(uint8_t aluOp, uint8_t funct) {
  if (aluOp == 0b00) {
    return ALUOperation::ADD;
  } if (aluOp == 0b01) {
    return ALUOperation::SUB;
  } if (aluOp == 0b10) {
    switch (funct) {
      case Funct::kADD: return ALUOperation::ADD;
      case Funct::kSUB: return ALUOperation::SUB;
      case Funct::kAND: return ALUOperation::AND;
      case Funct::kOR: return ALUOperation::OR;
      case Funct::kSLT: return ALUOperation::SLT;
      default: return ALUOperation::ADD;
    }
  } else if (aluOp == 0b11) {
    return ALUOperation::ADD; // placeholder
  }

  return ALUOperation::ADD;
}

// Pipeline stages
void CPU::fetch() {
  word_t pc = m_registers.getPc(); // NOLINT
  word_t instructionWord = m_memory.readWord(pc);
  m_currentInstruction = Instruction(instructionWord);
}

void CPU::decode() {
  m_pipeline.rs = m_currentInstruction.getRs();
  m_pipeline.rt = m_currentInstruction.getRt();
  m_pipeline.rd = m_currentInstruction.getRd();
  m_pipeline.funct = m_currentInstruction.getFunct();
  m_pipeline.immediate = m_currentInstruction.getImmediate();

  m_pipeline.read_data_1 = m_registers.read(m_pipeline.rs);
  m_pipeline.read_data_2 = m_registers.read(m_pipeline.rt);

  m_pipeline.control = generateControlSignals(m_currentInstruction.getOpcode());
}

void CPU::execute() {
  word_t aluInput2;
  uint8_t opcode = m_currentInstruction.getOpcode();
  ALUOperation aluOp;
  
  if (m_pipeline.control.ALUSrc) {
    // Use sign-extended immediate
    aluInput2 = static_cast<word_t>(static_cast<int32_t>(m_pipeline.immediate));

    if (opcode == Opcode::kANDI || opcode == Opcode::kORI) {
      aluOp = (opcode == Opcode::kANDI) ? ALUOperation::AND : ALUOperation::OR;
      aluInput2 = static_cast<word_t>(m_pipeline.immediate) & 0xFFFF;
    } else {
      aluOp = aluControl(m_pipeline.control.ALUOp, m_pipeline.funct);
    }
  } else {
    // Use register value
    aluInput2 = m_pipeline.read_data_2;
    aluOp = aluControl(m_pipeline.control.ALUOp, m_pipeline.funct);
  }
  
  // Execute ALU operation
  ALU::Result result = ALU::execute(aluOp, m_pipeline.read_data_1, aluInput2);
  m_pipeline.alu_result = result.value;
  m_pipeline.zero_flag = result.zero;

  m_pipeline.write_reg = m_pipeline.control.RegDst ? m_pipeline.rd : m_pipeline.rt;
  m_pipeline.mem_write_data = m_pipeline.read_data_2;
  
  word_t pc = m_registers.getPc(); // NOLINT

  if (m_pipeline.control.Branch) {
    bool takeBranch = false;

    if (opcode == Opcode::kBEQ) {
      takeBranch = m_pipeline.zero_flag;
    } else if (opcode == Opcode::kBNE) {
      takeBranch = !m_pipeline.zero_flag;
    }

    if (takeBranch) {
      auto signExtendedImm = static_cast<word_t>(static_cast<int32_t>(m_pipeline.immediate));
      m_registers.setPc(pc + (signExtendedImm << 2));
    }
  }

  if (m_pipeline.control.Jump) {
    address_t addr = m_currentInstruction.getAddress();
    word_t jumpAddr = ((pc + 4) & 0xF0000000) | (addr << 2);

    if (opcode == Opcode::kJAL) {
      m_pipeline.wb_data = pc + 4;
      m_pipeline.write_reg = 31; // $ra
    }

    m_registers.setPc(jumpAddr - 4);
  }

}

void CPU::mMemoryaccess() {
  if (m_pipeline.control.MemRead) {
    m_pipeline.mem_read_data = m_memory.readWord(m_pipeline.alu_result);
  }

  if (m_pipeline.control.MemWrite) {
    m_memory.writeWord(m_pipeline.alu_result, m_pipeline.mem_write_data);
  }
}

void CPU::writeBack() {
  if (m_pipeline.control.RegWrite) {
    word_t writeData;
    if (m_pipeline.control.MemToReg) {
      writeData = m_pipeline.mem_read_data;
    } else if (m_pipeline.control.Jump && m_currentInstruction.getOpcode() == Opcode::kJAL) {
      writeData = m_pipeline.wb_data;
    } else {
      writeData = m_pipeline.alu_result;
    }

    m_registers.write(m_pipeline.write_reg, writeData);
  }

  m_registers.incrementPc();

  m_pipeline.clear();
}
} // namespace ez_arch
