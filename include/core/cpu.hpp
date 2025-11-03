#pragma once

#include "register_file.hpp"
#include "memory.hpp"
#include "instruction.hpp"
#include <functional>

namespace ez_arch {

enum class ExecutionStage {
    FETCH,
    DECODE,
    EXECUTE,
    MEMORY_ACCESS,
    WRITE_BACK
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
    ExecutionStage get_current_stage() const { return current_stage_; }
    void step_stage(); // Execute one pipeline stage
    
    // State access
    const RegisterFile& get_registers() const { return registers_; }
    const Memory& get_memory() const { return memory_; }
    bool is_halted() const { return halted_; }
    
    // Callbacks for visualization
    using StageCallback = std::function<void(ExecutionStage)>;
    void set_stage_callback(StageCallback callback) { stage_callback_ = callback; }
    
private:
    RegisterFile registers_;
    Memory memory_;
    
    Instruction current_instruction_;
    ExecutionStage current_stage_;
    bool halted_;
    
    StageCallback stage_callback_;
    
    // Pipeline stages
    void fetch();
    void decode();
    void execute();
    void memory_access();
    void write_back();
    
    // Instruction execution
    void execute_r_type(const Instruction& instr);
    void execute_i_type(const Instruction& instr);
    void execute_j_type(const Instruction& instr);
};

} // namespace ez_arch
