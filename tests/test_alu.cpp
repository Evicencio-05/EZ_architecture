#include "core/alu.hpp"

#include <gtest/gtest.h>

using namespace ez_arch;

// ADD Operation Tests
TEST(ALUTest, AddPositiveNumbers) {
  auto result = ALU::execute(ALUOperation::ADD, 10, 20);
  EXPECT_EQ(result.value, 30);
  EXPECT_FALSE(result.zero);
  EXPECT_FALSE(result.negative);
  EXPECT_FALSE(result.overflow);
}

TEST(ALUTest, AddResultZero) {
  auto result = ALU::execute(ALUOperation::ADD, 0, 0);
  EXPECT_EQ(result.value, 0);
  EXPECT_TRUE(result.zero);
  EXPECT_FALSE(result.negative);
  EXPECT_FALSE(result.overflow);
}

TEST(ALUTest, AddWithOverflow) {
  // 0x7FFFFFFF + 0x00000001 should overflow (max positive + 1)
  auto result = ALU::execute(ALUOperation::ADD, 0x7FFFFFFF, 0x00000001);
  EXPECT_EQ(result.value, 0x80000000);
  EXPECT_FALSE(result.zero);
  EXPECT_TRUE(result.negative);
  EXPECT_TRUE(result.overflow);
}

TEST(ALUTest, AddNegativeNumbers) {
  auto result = ALU::execute(
      ALUOperation::ADD, static_cast<word_t>(-10), static_cast<word_t>(-20));
  EXPECT_EQ(static_cast<int32_t>(result.value), -30);
  EXPECT_FALSE(result.zero);
  EXPECT_TRUE(result.negative);
}

// SUB Operation Tests
TEST(ALUTest, SubPositiveNumbers) {
  auto result = ALU::execute(ALUOperation::SUB, 50, 20);
  EXPECT_EQ(result.value, 30);
  EXPECT_FALSE(result.zero);
  EXPECT_FALSE(result.negative);
  EXPECT_FALSE(result.overflow);
}

TEST(ALUTest, SubResultZero) {
  auto result = ALU::execute(ALUOperation::SUB, 42, 42);
  EXPECT_EQ(result.value, 0);
  EXPECT_TRUE(result.zero);
  EXPECT_FALSE(result.negative);
  EXPECT_FALSE(result.overflow);
}

TEST(ALUTest, SubResultNegative) {
  auto result = ALU::execute(ALUOperation::SUB, 10, 20);
  EXPECT_EQ(static_cast<int32_t>(result.value), -10);
  EXPECT_FALSE(result.zero);
  EXPECT_TRUE(result.negative);
}

TEST(ALUTest, SubWithOverflow) {
  // 0x80000000 - 0x00000001 should overflow (most negative - 1)
  auto result = ALU::execute(ALUOperation::SUB, 0x80000000, 0x00000001);
  EXPECT_TRUE(result.overflow);
}

// AND Operation Tests
TEST(ALUTest, AndOperation) {
  auto result = ALU::execute(ALUOperation::AND, 0xFF00FF00, 0x0FF00FF0);
  EXPECT_EQ(result.value, 0x0F000F00);
  EXPECT_FALSE(result.zero);
  EXPECT_FALSE(result.overflow);
}

TEST(ALUTest, AndResultZero) {
  auto result = ALU::execute(ALUOperation::AND, 0xF0F0F0F0, 0x0F0F0F0F);
  EXPECT_EQ(result.value, 0);
  EXPECT_TRUE(result.zero);
  EXPECT_FALSE(result.overflow);
}

TEST(ALUTest, AndWithAllOnes) {
  auto result = ALU::execute(ALUOperation::AND, 0x12345678, 0xFFFFFFFF);
  EXPECT_EQ(result.value, 0x12345678);
  EXPECT_FALSE(result.zero);
}

// OR Operation Tests
TEST(ALUTest, OrOperation) {
  auto result = ALU::execute(ALUOperation::OR, 0xF0F0F0F0, 0x0F0F0F0F);
  EXPECT_EQ(result.value, 0xFFFFFFFF);
  EXPECT_FALSE(result.zero);
  EXPECT_FALSE(result.overflow);
}

TEST(ALUTest, OrResultZero) {
  auto result = ALU::execute(ALUOperation::OR, 0, 0);
  EXPECT_EQ(result.value, 0);
  EXPECT_TRUE(result.zero);
}

TEST(ALUTest, OrWithZero) {
  auto result = ALU::execute(ALUOperation::OR, 0x12345678, 0);
  EXPECT_EQ(result.value, 0x12345678);
  EXPECT_FALSE(result.zero);
}

// SLT (Set Less Than) Operation Tests
TEST(ALUTest, SltLessThan) {
  auto result = ALU::execute(ALUOperation::SLT, 5, 10);
  EXPECT_EQ(result.value, 1);
  EXPECT_FALSE(result.zero);
  EXPECT_FALSE(result.overflow);
}

TEST(ALUTest, SltNotLessThan) {
  auto result = ALU::execute(ALUOperation::SLT, 10, 5);
  EXPECT_EQ(result.value, 0);
  EXPECT_TRUE(result.zero);
  EXPECT_FALSE(result.overflow);
}

TEST(ALUTest, SltEqual) {
  auto result = ALU::execute(ALUOperation::SLT, 10, 10);
  EXPECT_EQ(result.value, 0);
  EXPECT_TRUE(result.zero);
}

TEST(ALUTest, SltSignedComparison) {
  // -1 < 1 in signed comparison
  auto result = ALU::execute(ALUOperation::SLT, static_cast<word_t>(-1), 1);
  EXPECT_EQ(result.value, 1);
  EXPECT_FALSE(result.zero);
}

TEST(ALUTest, SltNegativeNumbers) {
  // -10 < -5
  auto result = ALU::execute(
      ALUOperation::SLT, static_cast<word_t>(-10), static_cast<word_t>(-5));
  EXPECT_EQ(result.value, 1);
}

// NOR Operation Tests
TEST(ALUTest, NorOperation) {
  auto result = ALU::execute(ALUOperation::NOR, 0xF0F0F0F0, 0x0F0F0F0F);
  EXPECT_EQ(result.value, 0x00000000);
  EXPECT_TRUE(result.zero);
  EXPECT_FALSE(result.overflow);
}

TEST(ALUTest, NorOfZeros) {
  auto result = ALU::execute(ALUOperation::NOR, 0, 0);
  EXPECT_EQ(result.value, 0xFFFFFFFF);
  EXPECT_FALSE(result.zero);
  EXPECT_TRUE(result.negative);
}

TEST(ALUTest, NorResultNegative) {
  auto result = ALU::execute(ALUOperation::NOR, 0x00000000, 0x00000001);
  EXPECT_EQ(result.value, 0xFFFFFFFE);
  EXPECT_TRUE(result.negative);
}

// Edge Cases
TEST(ALUTest, MaxValues) {
  auto result = ALU::execute(ALUOperation::ADD, 0xFFFFFFFF, 0xFFFFFFFF);
  EXPECT_EQ(result.value, 0xFFFFFFFE);
}

TEST(ALUTest, ZeroOperands) {
  auto result = ALU::execute(ALUOperation::SUB, 0, 0);
  EXPECT_EQ(result.value, 0);
  EXPECT_TRUE(result.zero);
}
