#include <gtest/gtest.h>
#include "core/cpu.hpp"

using namespace ez_arch;

// Helper function to create R-type instruction
word_t makeRInstruction(uint8_t rs, uint8_t rt, uint8_t rd, uint8_t shamt, uint8_t funct) {
  return (0 << 26) | (rs << 21) | (rt << 16) | (rd << 11) | (shamt << 6) | funct;
}

// Helper function to create I-type instruction
word_t makeIInstruction(uint8_t opcode, uint8_t rs, uint8_t rt, int16_t imm) {
  return (opcode << 26) | (rs << 21) | (rt << 16) | (imm & 0xFFFF);
}

// Helper function to create J-type instruction
word_t makeJInstruction(uint8_t opcode, uint32_t address) {
  return (opcode << 26) | (address & 0x3FFFFFF);
}

// Initialization Tests
TEST(CPUTest, DefaultInitialization) {
  CPU cpu;
  EXPECT_FALSE(cpu.is_halted());
  EXPECT_EQ(cpu.get_current_stage(), ExecutionStage::FETCH);
  EXPECT_EQ(cpu.get_registers().get_pc(), 0);
}

TEST(CPUTest, LoadProgram) {
  CPU cpu;
  std::vector<word_t> program = {0x12345678, 0xABCDEF00};
  
  cpu.load_program(program);
  
  EXPECT_FALSE(cpu.is_halted());
  EXPECT_EQ(cpu.get_registers().get_pc(), 0);
  EXPECT_EQ(cpu.get_memory().read_word(0), 0x12345678);
  EXPECT_EQ(cpu.get_memory().read_word(4), 0xABCDEF00);
}

TEST(CPUTest, Reset) {
  CPU cpu;
  std::vector<word_t> program = {makeRInstruction(1, 2, 3, 0, Funct::kADD)};
  
  cpu.load_program(program);
  cpu.step();
  
  cpu.reset();
  
  EXPECT_FALSE(cpu.is_halted());
  EXPECT_EQ(cpu.get_registers().get_pc(), 0);
  EXPECT_EQ(cpu.get_current_stage(), ExecutionStage::FETCH);
}

// R-Type Instruction Tests
TEST(CPUTest, ExecuteADD) {
  CPU cpu;
  // add $3, $1, $2  (r3 = r1 + r2)
  std::vector<word_t> program = {makeRInstruction(1, 2, 3, 0, Funct::kADD)};
  
  cpu.load_program(program);
  cpu.get_registers().write(1, 10);
  cpu.get_registers().write(2, 20);
  
  cpu.step();
  
  EXPECT_EQ(cpu.get_registers().read(3), 30);
  EXPECT_EQ(cpu.get_registers().get_pc(), 4);
}

TEST(CPUTest, ExecuteSUB) {
  CPU cpu;
  // sub $5, $3, $4  (r5 = r3 - r4)
  std::vector<word_t> program = {make_r_instruction(3, 4, 5, 0, Funct::SUB)};
  
  cpu.load_program(program);
  cpu.get_registers().write(3, 50);
  cpu.get_registers().write(4, 20);
  
  cpu.step();
  
  EXPECT_EQ(cpu.get_registers().read(5), 30);
  EXPECT_EQ(cpu.get_registers().get_pc(), 4);
}

TEST(CPUTest, ExecuteAND) {
  CPU cpu;
  // and $6, $4, $5
  std::vector<word_t> program = {make_r_instruction(4, 5, 6, 0, Funct::AND)};
  
  cpu.load_program(program);
  cpu.get_registers().write(4, 0xFF00FF00);
  cpu.get_registers().write(5, 0x0FF00FF0);
  
  cpu.step();
  
  EXPECT_EQ(cpu.get_registers().read(6), 0x0F000F00);
}

TEST(CPUTest, ExecuteOR) {
  CPU cpu;
  // or $7, $5, $6
  std::vector<word_t> program = {make_r_instruction(5, 6, 7, 0, Funct::OR)};
  
  cpu.load_program(program);
  cpu.get_registers().write(5, 0xF0F0F0F0);
  cpu.get_registers().write(6, 0x0F0F0F0F);
  
  cpu.step();
  
  EXPECT_EQ(cpu.get_registers().read(7), 0xFFFFFFFF);
}

TEST(CPUTest, ExecuteSLT) {
  CPU cpu;
  // slt $8, $6, $7  (r8 = r6 < r7 ? 1 : 0)
  std::vector<word_t> program = {make_r_instruction(6, 7, 8, 0, Funct::SLT)};
  
  cpu.load_program(program);
  cpu.get_registers().write(6, 5);
  cpu.get_registers().write(7, 10);
  
  cpu.step();
  
  EXPECT_EQ(cpu.get_registers().read(8), 1);
}

TEST(CPUTest, ExecuteSLTNotLess) {
  CPU cpu;
  std::vector<word_t> program = {make_r_instruction(6, 7, 8, 0, Funct::SLT)};
  
  cpu.load_program(program);
  cpu.get_registers().write(6, 15);
  cpu.get_registers().write(7, 10);
  
  cpu.step();
  
  EXPECT_EQ(cpu.get_registers().read(8), 0);
}

TEST(CPUTest, RTypeDoesNotWriteToR0) {
  CPU cpu;
  // add $0, $1, $2 (attempt to write to r0)
  std::vector<word_t> program = {make_r_instruction(1, 2, 0, 0, Funct::ADD)};
  
  cpu.load_program(program);
  cpu.get_registers().write(1, 10);
  cpu.get_registers().write(2, 20);
  
  cpu.step();
  
  EXPECT_EQ(cpu.get_registers().read(0), 0);  // r0 must remain 0
}

// I-Type Instruction Tests
TEST(CPUTest, ExecuteADDI) {
  CPU cpu;
  // addi $2, $1, 100
  std::vector<word_t> program = {make_i_instruction(Opcode::ADDI, 1, 2, 100)};
  
  cpu.load_program(program);
  cpu.get_registers().write(1, 50);
  
  cpu.step();
  
  EXPECT_EQ(cpu.get_registers().read(2), 150);
}

TEST(CPUTest, ExecuteADDINegativeImmediate) {
  CPU cpu;
  // addi $2, $1, -10
  std::vector<word_t> program = {make_i_instruction(Opcode::ADDI, 1, 2, -10)};
  
  cpu.load_program(program);
  cpu.get_registers().write(1, 50);
  
  cpu.step();
  
  EXPECT_EQ(cpu.get_registers().read(2), 40);
}

TEST(CPUTest, ExecuteANDI) {
  CPU cpu;
  // andi $2, $1, 0xFF
  std::vector<word_t> program = {make_i_instruction(Opcode::ANDI, 1, 2, 0xFF)};
  
  cpu.load_program(program);
  cpu.get_registers().write(1, 0x12345678);
  
  cpu.step();
  
  EXPECT_EQ(cpu.get_registers().read(2), 0x00000078);
}

TEST(CPUTest, ExecuteORI) {
  CPU cpu;
  // ori $2, $1, 0xFF
  std::vector<word_t> program = {make_i_instruction(Opcode::ORI, 1, 2, 0xFF)};
  
  cpu.load_program(program);
  cpu.get_registers().write(1, 0x12345600);
  
  cpu.step();
  
  EXPECT_EQ(cpu.get_registers().read(2), 0x123456FF);
}

TEST(CPUTest, ExecuteLW) {
  CPU cpu;
  // lw $2, 8($1)  (load word from address r1 + 8)
  std::vector<word_t> program = {make_i_instruction(Opcode::LW, 1, 2, 8)};
  
  cpu.load_program(program);
  cpu.get_registers().write(1, 100);
  cpu.get_memory().write_word(108, 0xDEADBEEF);
  
  cpu.step();
  
  EXPECT_EQ(cpu.get_registers().read(2), 0xDEADBEEF);
}

TEST(CPUTest, ExecuteSW) {
  CPU cpu;
  // sw $2, 12($1)  (store word to address r1 + 12)
  std::vector<word_t> program = {make_i_instruction(Opcode::SW, 1, 2, 12)};
  
  cpu.load_program(program);
  cpu.get_registers().write(1, 100);
  cpu.get_registers().write(2, 0xCAFEBABE);
  
  cpu.step();
  
  EXPECT_EQ(cpu.get_memory().read_word(112), 0xCAFEBABE);
}

TEST(CPUTest, ExecuteBEQTaken) {
  CPU cpu;
  // beq $1, $2, 4  (branch if equal)
  std::vector<word_t> program = {make_i_instruction(Opcode::BEQ, 1, 2, 4)};
  
  cpu.load_program(program);
  cpu.get_registers().write(1, 42);
  cpu.get_registers().write(2, 42);
  
  cpu.step();
  
  // PC = 0 + 4 + (4 << 2) = 20
  EXPECT_EQ(cpu.get_registers().get_pc(), 20);
}

TEST(CPUTest, ExecuteBEQNotTaken) {
  CPU cpu;
  // beq $1, $2, 4
  std::vector<word_t> program = {make_i_instruction(Opcode::BEQ, 1, 2, 4)};
  
  cpu.load_program(program);
  cpu.get_registers().write(1, 42);
  cpu.get_registers().write(2, 43);
  
  cpu.step();
  
  // PC should just increment to next instruction
  EXPECT_EQ(cpu.get_registers().get_pc(), 4);
}

TEST(CPUTest, ExecuteBNETaken) {
  CPU cpu;
  // bne $1, $2, 4  (branch if not equal)
  std::vector<word_t> program = {make_i_instruction(Opcode::BNE, 1, 2, 4)};
  
  cpu.load_program(program);
  cpu.get_registers().write(1, 42);
  cpu.get_registers().write(2, 43);
  
  cpu.step();
  
  // PC = 0 + 4 + (4 << 2) = 20
  EXPECT_EQ(cpu.get_registers().get_pc(), 20);
}

TEST(CPUTest, ExecuteBNENotTaken) {
  CPU cpu;
  // bne $1, $2, 4
  std::vector<word_t> program = {make_i_instruction(Opcode::BNE, 1, 2, 4)};
  
  cpu.load_program(program);
  cpu.get_registers().write(1, 42);
  cpu.get_registers().write(2, 42);
  
  cpu.step();
  
  EXPECT_EQ(cpu.get_registers().get_pc(), 4);
}

// J-Type Instruction Tests
TEST(CPUTest, ExecuteJ) {
  CPU cpu;
  // j 0x100000
  std::vector<word_t> program = {make_j_instruction(Opcode::J, 0x100000)};
  
  cpu.load_program(program);
  cpu.step();
  
  // Jump address = (PC+4 & 0xF0000000) | (0x100000 << 2)
  EXPECT_EQ(cpu.get_registers().get_pc(), 0x00400000);
}

TEST(CPUTest, ExecuteJAL) {
  CPU cpu;
  // jal 0x200000  (jump and link)
  std::vector<word_t> program = {make_j_instruction(Opcode::JAL, 0x200000)};
  
  cpu.load_program(program);
  cpu.step();
  
  // Return address (PC + 4) should be in $ra (register 31)
  EXPECT_EQ(cpu.get_registers().read(31), 4);
  EXPECT_EQ(cpu.get_registers().get_pc(), 0x00800000);
}

// Multi-Instruction Tests
TEST(CPUTest, ExecuteMultipleInstructions) {
  CPU cpu;
  std::vector<word_t> program = {
    make_r_instruction(1, 2, 3, 0, Funct::ADD),  // r3 = r1 + r2
    make_r_instruction(3, 4, 5, 0, Funct::SUB),  // r5 = r3 - r4
    make_r_instruction(5, 0, 6, 0, Funct::ADD)   // r6 = r5 + 0
  };
  
  cpu.load_program(program);
  cpu.get_registers().write(1, 10);
  cpu.get_registers().write(2, 20);
  cpu.get_registers().write(4, 5);
  
  cpu.step();  // r3 = 30
  cpu.step();  // r5 = 25
  cpu.step();  // r6 = 25
  
  EXPECT_EQ(cpu.get_registers().read(3), 30);
  EXPECT_EQ(cpu.get_registers().read(5), 25);
  EXPECT_EQ(cpu.get_registers().read(6), 25);
  EXPECT_EQ(cpu.get_registers().get_pc(), 12);
}

TEST(CPUTest, MemoryOperationsSequence) {
  CPU cpu;
  std::vector<word_t> program = {
    make_i_instruction(Opcode::ADDI, 0, 1, 100),  // r1 = 100
    make_i_instruction(Opcode::ADDI, 0, 2, 42),   // r2 = 42
    make_i_instruction(Opcode::SW, 1, 2, 0),      // mem[r1] = r2
    make_i_instruction(Opcode::LW, 1, 3, 0)       // r3 = mem[r1]
  };
  
  cpu.load_program(program);
  
  cpu.step();  // r1 = 100
  cpu.step();  // r2 = 42
  cpu.step();  // mem[100] = 42
  cpu.step();  // r3 = mem[100] = 42
  
  EXPECT_EQ(cpu.get_registers().read(1), 100);
  EXPECT_EQ(cpu.get_registers().read(2), 42);
  EXPECT_EQ(cpu.get_registers().read(3), 42);
  EXPECT_EQ(cpu.get_memory().read_word(100), 42);
}

// Pipeline Stage Tests
TEST(CPUTest, StepStageProgression) {
  CPU cpu;
  std::vector<word_t> program = {make_r_instruction(1, 2, 3, 0, Funct::ADD)};
  cpu.load_program(program);
  
  EXPECT_EQ(cpu.get_current_stage(), ExecutionStage::FETCH);
  
  cpu.step_stage();
  EXPECT_EQ(cpu.get_current_stage(), ExecutionStage::DECODE);
  
  cpu.step_stage();
  EXPECT_EQ(cpu.get_current_stage(), ExecutionStage::EXECUTE);
  
  cpu.step_stage();
  EXPECT_EQ(cpu.get_current_stage(), ExecutionStage::MEMORY_ACCESS);
  
  cpu.step_stage();
  EXPECT_EQ(cpu.get_current_stage(), ExecutionStage::WRITE_BACK);
  
  cpu.step_stage();
  EXPECT_EQ(cpu.get_current_stage(), ExecutionStage::FETCH);
}

TEST(CPUTest, StepStageCallback) {
  CPU cpu;
  std::vector<word_t> program = {make_r_instruction(1, 2, 3, 0, Funct::ADD)};
  cpu.load_program(program);
  
  int callbackCount = 0;
  ExecutionStage lastStage;
  
  cpu.set_stage_callback([&](ExecutionStage stage) {
    callbackCount++;
    lastStage = stage;
  });
  
  cpu.step_stage();
  
  EXPECT_EQ(callbackCount, 1);
  EXPECT_EQ(lastStage, ExecutionStage::FETCH);
}

// Run Tests
TEST(CPUTest, RunUntilHalt) {
  CPU cpu;
  std::vector<word_t> program = {
    make_r_instruction(1, 2, 3, 0, Funct::ADD),
    make_r_instruction(3, 4, 5, 0, Funct::SUB),
    0x00000000  // NOP/halt
  };
  
  cpu.load_program(program);
  cpu.get_registers().write(1, 10);
  cpu.get_registers().write(2, 20);
  cpu.get_registers().write(4, 5);
  
  cpu.run();
  
  EXPECT_TRUE(cpu.is_halted());
  EXPECT_EQ(cpu.get_registers().read(3), 30);
  EXPECT_EQ(cpu.get_registers().read(5), 25);
}

TEST(CPUTest, HaltedCPUDoesNotStep) {
  CPU cpu;
  std::vector<word_t> program = {0x00000000};
  
  cpu.load_program(program);
  cpu.run();
  
  EXPECT_TRUE(cpu.is_halted());
  word_t pcBefore = cpu.get_registers().get_pc();
  
  cpu.step();
  
  EXPECT_EQ(cpu.get_registers().get_pc(), pcBefore);
}

// Edge Cases
TEST(CPUTest, EmptyProgram) {
  CPU cpu;
  std::vector<word_t> program = {};
  
  cpu.load_program(program);
  EXPECT_FALSE(cpu.is_halted());
}

TEST(CPUTest, SingleInstructionProgram) {
  CPU cpu;
  std::vector<word_t> program = {make_r_instruction(1, 2, 3, 0, Funct::ADD)};
  
  cpu.load_program(program);
  cpu.get_registers().write(1, 5);
  cpu.get_registers().write(2, 7);
  
  cpu.step();
  
  EXPECT_EQ(cpu.get_registers().read(3), 12);
}

TEST(CPUTest, BranchBackwards) {
  CPU cpu;
  // beq $1, $2, -1  (branch backwards)
  std::vector<word_t> program = {make_i_instruction(Opcode::BEQ, 1, 2, -1)};
  
  cpu.load_program(program);
  cpu.get_registers().write(1, 42);
  cpu.get_registers().write(2, 42);
  
  cpu.step();
  
  // PC = 0 + 4 + (-1 << 2) = 0
  EXPECT_EQ(cpu.get_registers().get_pc(), 0);
}

TEST(CPUTest, LoadWordWithNegativeOffset) {
  CPU cpu;
  // lw $2, -4($1)
  std::vector<word_t> program = {make_i_instruction(Opcode::LW, 1, 2, -4)};
  
  cpu.load_program(program);
  cpu.get_registers().write(1, 104);
  cpu.get_memory().write_word(100, 0x12345678);
  
  cpu.step();
  
  EXPECT_EQ(cpu.get_registers().read(2), 0x12345678);
}

TEST(CPUTest, StoreWordWithNegativeOffset) {
  CPU cpu;
  // sw $2, -4($1)
  std::vector<word_t> program = {make_i_instruction(Opcode::SW, 1, 2, -4)};
  
  cpu.load_program(program);
  cpu.get_registers().write(1, 104);
  cpu.get_registers().write(2, 0xABCDEF00);
  
  cpu.step();
  
  EXPECT_EQ(cpu.get_memory().read_word(100), 0xABCDEF00);
}

// Control Signals and Pipeline Tests
TEST(CPUTest, StageToString) {
  EXPECT_EQ(stageToString(ExecutionStage::FETCH), "Fetch (IF)");
  EXPECT_EQ(stageToString(ExecutionStage::DECODE), "Decode (ID)");
  EXPECT_EQ(stageToString(ExecutionStage::EXECUTE), "Execute (EXE)");
  EXPECT_EQ(stageToString(ExecutionStage::MEMORY_ACCESS), "Memory Access (MEM)");
  EXPECT_EQ(stageToString(ExecutionStage::WRITE_BACK), "Write Back (WB)");
}

TEST(CPUTest, HaltDetection) {
  CPU cpu;
  // Program with explicit halt (NOP = 0x00000000)
  std::vector<word_t> program = {
    make_r_instruction(1, 2, 3, 0, Funct::ADD),
    0x00000000  // Halt instruction
  };
  
  cpu.load_program(program);
  cpu.get_registers().write(1, 10);
  cpu.get_registers().write(2, 20);
  
  EXPECT_FALSE(cpu.is_halted());
  
  cpu.step();  // Execute ADD
  EXPECT_FALSE(cpu.is_halted());
  EXPECT_EQ(cpu.get_registers().read(3), 30);
  
  cpu.step();  // Fetch halt instruction
  EXPECT_TRUE(cpu.is_halted());
}

TEST(CPUTest, PipelineStagesAfterReset) {
  CPU cpu;
  std::vector<word_t> program = {make_r_instruction(1, 2, 3, 0, Funct::ADD)};
  
  cpu.load_program(program);
  cpu.step_stage();  // Move to DECODE
  cpu.step_stage();  // Move to EXECUTE
  
  EXPECT_EQ(cpu.get_current_stage(), ExecutionStage::EXECUTE);
  
  cpu.reset();
  
  EXPECT_EQ(cpu.get_current_stage(), ExecutionStage::FETCH);
  EXPECT_FALSE(cpu.is_halted());
}

TEST(CPUTest, ControlSignalsRType) {
  CPU cpu;
  // R-type instructions should have specific control signals
  std::vector<word_t> program = {make_r_instruction(1, 2, 3, 0, Funct::ADD)};
  
  cpu.load_program(program);
  cpu.get_registers().write(1, 10);
  cpu.get_registers().write(2, 20);
  
  // Execute the instruction
  cpu.step();
  
  // Verify the result - control signals are internal but result should be correct
  EXPECT_EQ(cpu.get_registers().read(3), 30);
  EXPECT_EQ(cpu.get_registers().get_pc(), 4);
}

TEST(CPUTest, ControlSignalsITypeADDI) {
  CPU cpu;
  std::vector<word_t> program = {make_i_instruction(Opcode::ADDI, 1, 2, 50)};
  
  cpu.load_program(program);
  cpu.get_registers().write(1, 100);
  
  cpu.step();
  
  EXPECT_EQ(cpu.get_registers().read(2), 150);
}

TEST(CPUTest, ControlSignalsLoadStore) {
  CPU cpu;
  std::vector<word_t> program = {
    make_i_instruction(Opcode::ADDI, 0, 1, 100),  // r1 = 100 (address)
    make_i_instruction(Opcode::ADDI, 0, 2, 42),   // r2 = 42 (data)
    make_i_instruction(Opcode::SW, 1, 2, 0),      // mem[r1] = r2
    make_i_instruction(Opcode::LW, 1, 3, 0)       // r3 = mem[r1]
  };
  
  cpu.load_program(program);
  
  cpu.step();  // r1 = 100
  cpu.step();  // r2 = 42
  cpu.step();  // Store r2 to memory[100]
  cpu.step();  // Load from memory[100] to r3
  
  EXPECT_EQ(cpu.get_registers().read(1), 100);
  EXPECT_EQ(cpu.get_registers().read(2), 42);
  EXPECT_EQ(cpu.get_registers().read(3), 42);
  EXPECT_EQ(cpu.get_memory().read_word(100), 42);
}

// Complex Programs
TEST(CPUTest, SimpleLoop) {
  CPU cpu;
  std::vector<word_t> program = {
    // Loop: increment r1, decrement r2, branch if r2 != 0
    make_i_instruction(Opcode::ADDI, 1, 1, 1),    // 0: r1 = r1 + 1
    make_i_instruction(Opcode::ADDI, 2, 2, -1),   // 4: r2 = r2 - 1
    make_i_instruction(Opcode::BNE, 2, 0, -3),    // 8: if r2 != 0, goto 0 (offset: (0 - 12) / 4 = -3)
    0x00000000                                     // 12: halt
  };
  
  cpu.load_program(program);
  cpu.get_registers().write(1, 0);
  cpu.get_registers().write(2, 5);
  
  cpu.run();
  
  EXPECT_EQ(cpu.get_registers().read(1), 5);
  EXPECT_EQ(cpu.get_registers().read(2), 0);
}
