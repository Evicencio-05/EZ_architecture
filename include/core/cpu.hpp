#pragma once

#include "alu.hpp"
#include "instruction.hpp"
#include "memory.hpp"
#include "register_file.hpp"

#include <cstdint>
#include <functional>
#include <string_view>
#include <utility>

namespace ez_arch {

enum class ExecutionStage : uint8_t {
  FETCH,
  DECODE,
  EXECUTE,
  MEMORY_ACCESS,
  WRITE_BACK
};

constexpr std::string_view stageToString(ExecutionStage stage) {
  switch (stage) {
  case ExecutionStage::FETCH:
    return "Fetch (IF)";
  case ExecutionStage::DECODE:
    return "Decode (ID)";
  case ExecutionStage::EXECUTE:
    return "Execute (EXE)";
  case ExecutionStage::MEMORY_ACCESS:
    return "Memory Access (MEM)";
  case ExecutionStage::WRITE_BACK:
    return "Write Back (WB)";
  default:
    return "Stage unknown";
  };
}

struct ControlSignals {
  bool RegDst;
  bool Jump;
  bool Branch;
  bool MemRead;
  bool MemToReg;
  uint8_t ALUOp;
  bool MemWrite;
  bool ALUSrc;
  bool RegWrite;

  void clear() {
    RegDst = false;
    Jump = false;
    Branch = false;
    MemRead = false;
    MemToReg = false;
    ALUOp = 0;
    MemWrite = false;
    ALUSrc = false;
    RegWrite = false;
  }
};

struct PipelineRegisters {
  // ID/EX (Decode/Execute)
  word_t read_data_1; // Register rs value
  word_t read_data_2; // Register rt value
  int16_t immediate;  // Sign-extended immediate
  register_id_t rs;
  register_id_t rt;
  register_id_t rd;
  uint8_t funct; // R type
  ControlSignals control;

  // EX/MEM (Execute/Memory)
  word_t alu_result;
  word_t mem_write_data;   // Data to write to memory (from rt)
  register_id_t write_reg; // Selected by RegDst
  bool zero_flag;          // From ALU

  // MEM/WB (Memory/WriteBack)
  word_t mem_read_data;
  word_t wb_data; // Final data to write back

  void clear() {
    read_data_1 = 0;
    read_data_2 = 0;
    immediate = 0;
    rs = rt = rd = 0;
    funct = 0;
    control.clear();
    alu_result = 0;
    mem_write_data = 0;
    write_reg = 0;
    zero_flag = false;
  }
};

class CPU {
public:
  CPU();

  // Execution control
  void loadProgram(const std::vector<word_t>& program);
  void step(); // Execute one instruction
  void run();  // Execute until halt
  void reset();

  // Step-by-step execution (for educational purposes)
  [[nodiscard]] ExecutionStage getCurrentStage() const {
    return m_currentStage;
  }
  void stepStage(); // Execute one pipeline stage

  // State access
  [[nodiscard]] const RegisterFile& getRegisters() const { return m_registers; }
  RegisterFile& getRegisters() { return m_registers; }
  [[nodiscard]] const Memory& getMemory() const { return m_memory; }
  Memory& getMemory() { return m_memory; }
  [[nodiscard]] bool isHalted() const { return m_halted; }

  // Callbacks for visualization
  using StageCallback = std::function<void(ExecutionStage)>;
  void setStageCallback(StageCallback callback) {
    m_stageCallback = std::move(callback);
  }

  [[nodiscard]] Instruction getCurrentInstruction() const {
    return m_currentInstruction;
  }

private:
  RegisterFile m_registers;
  Memory m_memory;

  Instruction m_currentInstruction;
  ExecutionStage m_currentStage {ExecutionStage::FETCH};
  bool m_halted {false};

  StageCallback m_stageCallback;

  PipelineRegisters m_pipeline;

  void clearPipeline();
  static ControlSignals generateControlSignals(uint8_t opcode);
  static ALUOperation aluControl(uint8_t aluOp, uint8_t funct);

  // Pipeline stages
  void fetch();
  void decode();
  void execute();
  void mMemoryaccess();
  void writeBack();

  // Instruction execution
  void executeRType(const Instruction& instr);
  void executeIType(const Instruction& instr);
  void executeJType(const Instruction& instr);
};

} // namespace ez_arch
