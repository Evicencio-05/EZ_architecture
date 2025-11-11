#pragma once

#include "register_file.hpp"
#include "memory.hpp"
#include "instruction.hpp"
#include "alu.hpp"
#include <functional>
#include <string>
#include <string_view>

namespace ez_arch {

enum class ExecutionStage {
    FETCH,
    DECODE,
    EXECUTE,
    MEMORY_ACCESS,
    WRITE_BACK
};

constexpr std::string_view stageToString(ExecutionStage stage) {
  switch (stage) {
    case ExecutionStage::FETCH: return "Fetch (IF)";
    case ExecutionStage::DECODE: return "Decode (ID)";
    case ExecutionStage::EXECUTE: return "Execute (EXE)";
    case ExecutionStage::MEMORY_ACCESS: return "Memory Access (MEM)";
    case ExecutionStage::WRITE_BACK: return "Write Back (WB)";
    default: return "Stage unknown";
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
  word_t read_data_1;       // Register rs value
  word_t read_data_2;       // Register rt value
  int16_t immediate;        // Sign-extended immediate
  register_id_t rs;
  register_id_t rt;
  register_id_t rd;
  uint8_t funct;            // R type
  ControlSignals control;

  // EX/MEM (Execute/Memory)
  word_t alu_result;
  word_t mem_write_data;    // Data to write to memory (from rt)
  register_id_t write_reg;  // Selected by RegDst
  bool zero_flag;           // From ALU
  
  // MEM/WB (Memory/WriteBack)
  word_t mem_read_data;
  word_t wb_data;           // Final data to write back
  
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
    zero_flag = 0;
  }
};

class CPU {
public:
    CPU();
    
    // Execution control
    void load_program(const std::vector<word_t>& program);
    void step(); // Execute one instruction
    void run();  // Execute until halt
    void reset();
    
    // Step-by-step execution (for educational purposes)
    ExecutionStage get_current_stage() const { return m_currentStage; }
    void step_stage(); // Execute one pipeline stage
    
    // State access
    const RegisterFile& get_registers() const { return m_registers; }
    RegisterFile& get_registers() { return m_registers; }
    const Memory& get_memory() const { return m_memory; }
    Memory& get_memory() { return m_memory; }
    bool is_halted() const { return m_halted; }
    
    // Callbacks for visualization
    using StageCallback = std::function<void(ExecutionStage)>;
    void set_stage_callback(StageCallback callback) { m_stageCallback = callback; }
    
private:
    RegisterFile m_registers;
    Memory m_memory;
    
    Instruction m_currentInstruction;
    ExecutionStage m_currentStage;
    bool m_halted;
    
    StageCallback m_stageCallback;
    
    PipelineRegisters m_pipeline;

    void clear_pipeline();
    ControlSignals generate_control_signals(uint8_t opcode);
    ALUOperation alu_control(uint8_t ALUOp, uint8_t funct);

    // Pipeline stages
    void fetch();
    void decode();
    void execute();
    void m_memoryaccess();
    void write_back();
    
    // Instruction execution
    void execute_r_type(const Instruction& instr);
    void execute_i_type(const Instruction& instr);
    void execute_j_type(const Instruction& instr);
};

} // namespace ez_arch
