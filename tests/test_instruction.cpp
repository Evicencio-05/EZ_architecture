#include "core/instruction.hpp"

#include <gtest/gtest.h>
#include <tuple>

using namespace ez_arch;

// R-Type Instruction Tests
TEST(InstructionTest, RTypeFormat) {
  // R-type: opcode=0, rs=1, rt=2, rd=3, shamt=0, funct=0x20 (ADD)
  word_t raw = 0x00221820; // add $3, $1, $2
  Instruction instr(raw);

  EXPECT_EQ(instr.getFormat(), InstructionFormat::R_TYPE);
  EXPECT_EQ(instr.getOpcode(), 0);
  EXPECT_EQ(instr.getRs(), 1);
  EXPECT_EQ(instr.getRt(), 2);
  EXPECT_EQ(instr.getRd(), 3);
  EXPECT_EQ(instr.getShamt(), 0);
  EXPECT_EQ(instr.getFunct(), 0x20);
}

TEST(InstructionTest, RTypeWithShamt) {
  // sll $2, $1, 4 (shift left logical)
  // opcode=0, rs=0, rt=1, rd=2, shamt=4, funct=0
  word_t raw = 0x00011100;
  Instruction instr(raw);

  EXPECT_EQ(instr.getFormat(), InstructionFormat::R_TYPE);
  EXPECT_EQ(instr.getRt(), 1);
  EXPECT_EQ(instr.getRd(), 2);
  EXPECT_EQ(instr.getShamt(), 4);
  EXPECT_EQ(instr.getFunct(), 0);
}

TEST(InstructionTest, RTypeSubtract) {
  // sub $5, $3, $4
  // opcode=0, rs=3, rt=4, rd=5, shamt=0, funct=0x22
  word_t raw = 0x00642822;
  Instruction instr(raw);

  EXPECT_EQ(instr.getFormat(), InstructionFormat::R_TYPE);
  EXPECT_EQ(instr.getRs(), 3);
  EXPECT_EQ(instr.getRt(), 4);
  EXPECT_EQ(instr.getRd(), 5);
  EXPECT_EQ(instr.getFunct(), 0x22);
}

TEST(InstructionTest, RTypeAllZeros) {
  word_t raw = 0x00000000;
  Instruction instr(raw);

  EXPECT_EQ(instr.getFormat(), InstructionFormat::R_TYPE);
  EXPECT_EQ(instr.getRs(), 0);
  EXPECT_EQ(instr.getRt(), 0);
  EXPECT_EQ(instr.getRd(), 0);
  EXPECT_EQ(instr.getShamt(), 0);
  EXPECT_EQ(instr.getFunct(), 0);
}

// I-Type Instruction Tests
TEST(InstructionTest, ITypeFormat) {
  // addi $2, $1, 100
  // opcode=8, rs=1, rt=2, immediate=100
  word_t raw = 0x20220064;
  Instruction instr(raw);

  EXPECT_EQ(instr.getFormat(), InstructionFormat::I_TYPE);
  EXPECT_EQ(instr.getOpcode(), 8);
  EXPECT_EQ(instr.getRs(), 1);
  EXPECT_EQ(instr.getRt(), 2);
  EXPECT_EQ(instr.getImmediate(), 100);
}

TEST(InstructionTest, ITypeLoadWord) {
  // lw $2, 8($1)
  // opcode=0x23, rs=1, rt=2, offset=8
  word_t raw = 0x8C220008;
  Instruction instr(raw);

  EXPECT_EQ(instr.getFormat(), InstructionFormat::I_TYPE);
  EXPECT_EQ(instr.getOpcode(), 0x23);
  EXPECT_EQ(instr.getRs(), 1);
  EXPECT_EQ(instr.getRt(), 2);
  EXPECT_EQ(instr.getImmediate(), 8);
}

TEST(InstructionTest, ITypeStoreWord) {
  // sw $3, 12($2)
  // opcode=0x2B, rs=2, rt=3, offset=12
  word_t raw = 0xAC43000C;
  Instruction instr(raw);

  EXPECT_EQ(instr.getFormat(), InstructionFormat::I_TYPE);
  EXPECT_EQ(instr.getOpcode(), 0x2B);
  EXPECT_EQ(instr.getRs(), 2);
  EXPECT_EQ(instr.getRt(), 3);
  EXPECT_EQ(instr.getImmediate(), 12);
}

TEST(InstructionTest, ITypeNegativeImmediate) {
  // addi $2, $1, -1
  // opcode=8, rs=1, rt=2, immediate=-1
  word_t raw = 0x2022FFFF;
  Instruction instr(raw);

  EXPECT_EQ(instr.getFormat(), InstructionFormat::I_TYPE);
  EXPECT_EQ(instr.getImmediate(), -1);
}

TEST(InstructionTest, ITypeBranchEqual) {
  // beq $1, $2, offset
  // opcode=4, rs=1, rt=2, offset=16
  word_t raw = 0x10220010;
  Instruction instr(raw);

  EXPECT_EQ(instr.getFormat(), InstructionFormat::I_TYPE);
  EXPECT_EQ(instr.getOpcode(), 4);
  EXPECT_EQ(instr.getRs(), 1);
  EXPECT_EQ(instr.getRt(), 2);
  EXPECT_EQ(instr.getImmediate(), 16);
}

TEST(InstructionTest, ITypeMaxImmediate) {
  // Test maximum positive immediate value
  word_t raw = 0x20007FFF; // addi $0, $0, 32767
  Instruction instr(raw);

  EXPECT_EQ(instr.getImmediate(), 32767);
}

TEST(InstructionTest, ITypeMinImmediate) {
  // Test minimum (most negative) immediate value
  word_t raw = 0x20008000; // addi $0, $0, -32768
  Instruction instr(raw);

  EXPECT_EQ(instr.getImmediate(), -32768);
}

// J-Type Instruction Tests
TEST(InstructionTest, JTypeFormat) {
  // j 0x100000 (jump to address)
  // opcode=2, address=0x100000
  word_t raw = 0x08100000;
  Instruction instr(raw);

  EXPECT_EQ(instr.getFormat(), InstructionFormat::J_TYPE);
  EXPECT_EQ(instr.getOpcode(), 2);
  EXPECT_EQ(instr.getAddress(), 0x100000);
}

TEST(InstructionTest, JTypeJumpAndLink) {
  // jal 0x200000
  // opcode=3, address=0x200000
  word_t raw = 0x0C200000;
  Instruction instr(raw);

  EXPECT_EQ(instr.getFormat(), InstructionFormat::J_TYPE);
  EXPECT_EQ(instr.getOpcode(), 3);
  EXPECT_EQ(instr.getAddress(), 0x200000);
}

TEST(InstructionTest, JTypeMaxAddress) {
  // Test maximum address value (26 bits)
  word_t raw = 0x0BFFFFFF; // jal with max address
  Instruction instr(raw);

  EXPECT_EQ(instr.getFormat(), InstructionFormat::J_TYPE);
  EXPECT_EQ(instr.getAddress(), 0x3FFFFFF);
}

TEST(InstructionTest, JTypeZeroAddress) {
  word_t raw = 0x08000000; // j 0
  Instruction instr(raw);

  EXPECT_EQ(instr.getFormat(), InstructionFormat::J_TYPE);
  EXPECT_EQ(instr.getAddress(), 0);
}

// Raw Instruction Tests
TEST(InstructionTest, GetRawValue) {
  word_t raw = 0x12345678;
  Instruction instr(raw);

  EXPECT_EQ(instr.getRaw(), raw);
}

TEST(InstructionTest, PreserveRawValue) {
  word_t raw = 0xABCDEF01;
  Instruction instr(raw);

  // Verify that getting individual fields doesn't corrupt the raw value
  std::ignore = instr.getOpcode();
  std::ignore = instr.getRs();
  std::ignore = instr.getRt();

  EXPECT_EQ(instr.getRaw(), raw);
}

// Opcode Extraction Tests
TEST(InstructionTest, OpcodeExtraction) {
  // Test various opcodes
  Instruction instr1(0x00000000); // opcode = 0
  EXPECT_EQ(instr1.getOpcode(), 0);

  Instruction instr2(0x20000000); // opcode = 8
  EXPECT_EQ(instr2.getOpcode(), 8);

  Instruction instr3(0xFC000000); // opcode = 63 (max 6-bit value)
  EXPECT_EQ(instr3.getOpcode(), 63);
}

// Register Field Tests
TEST(InstructionTest, RegisterFieldsMaxValues) {
  // Test with all register fields set to max (31)
  word_t raw = 0x03FFFFFF; // rs=31, rt=31, rd=31
  Instruction instr(raw);

  EXPECT_EQ(instr.getRs(), 31);
  EXPECT_EQ(instr.getRt(), 31);
  EXPECT_EQ(instr.getRd(), 31);
}

TEST(InstructionTest, ShamtMaxValue) {
  // shamt is 5 bits, max value is 31
  word_t raw = 0x000007C0; // shamt = 31
  Instruction instr(raw);

  EXPECT_EQ(instr.getShamt(), 31);
}

TEST(InstructionTest, FunctMaxValue) {
  // funct is 6 bits, max value is 63
  word_t raw = 0x0000003F; // funct = 63
  Instruction instr(raw);

  EXPECT_EQ(instr.getFunct(), 63);
}

// Edge Cases
TEST(InstructionTest, AllOnesInstruction) {
  word_t raw = 0xFFFFFFFF;
  Instruction instr(raw);

  EXPECT_EQ(instr.getOpcode(), 63);
  EXPECT_EQ(instr.getRs(), 31);
  EXPECT_EQ(instr.getRt(), 31);
  EXPECT_EQ(instr.getRd(), 31);
  EXPECT_EQ(instr.getShamt(), 31);
  EXPECT_EQ(instr.getFunct(), 63);
  EXPECT_EQ(instr.getImmediate(), -1);
  EXPECT_EQ(instr.getAddress(), 0x3FFFFFF);
}

TEST(InstructionTest, AlternatingBits) {
  word_t raw = 0xAAAAAAAA;
  Instruction instr(raw);

  EXPECT_EQ(instr.getRaw(), 0xAAAAAAAA);
  // Verify fields extract correctly even with alternating bits
  EXPECT_EQ(instr.getOpcode(), 42); // 0b101010
}

// ToString Tests
TEST(InstructionTest, ToStringNotEmpty) {
  Instruction instr(0x00221820);
  std::string str = instr.toString();

  EXPECT_FALSE(str.empty());
  EXPECT_NE(str.find("0x"), std::string::npos); // Should contain hex prefix
}

TEST(InstructionTest, ToStringContainsRaw) {
  Instruction instr(0x12345678);
  std::string str = instr.toString();

  EXPECT_NE(str.find("12345678"), std::string::npos);
}
