#include <gtest/gtest.h>
#include "core/memory.hpp"

using namespace ez_arch;

// Word Operations Tests
TEST(MemoryTest, WriteAndReadWord) {
  Memory mem(1024);
  mem.write_word(0, 0x12345678);
  EXPECT_EQ(mem.read_word(0), 0x12345678);
}

TEST(MemoryTest, WriteWordBigEndian) {
  Memory mem(1024);
  mem.write_word(0, 0x12345678);
  EXPECT_EQ(mem.read_byte(0), 0x12);  // Most significant byte first
  EXPECT_EQ(mem.read_byte(1), 0x34);
  EXPECT_EQ(mem.read_byte(2), 0x56);
  EXPECT_EQ(mem.read_byte(3), 0x78);  // Least significant byte last
}

TEST(MemoryTest, WriteWordAtDifferentAddresses) {
  Memory mem(1024);
  mem.write_word(0, 0xAAAAAAAA);
  mem.write_word(4, 0xBBBBBBBB);
  mem.write_word(8, 0xCCCCCCCC);
  
  EXPECT_EQ(mem.read_word(0), 0xAAAAAAAA);
  EXPECT_EQ(mem.read_word(4), 0xBBBBBBBB);
  EXPECT_EQ(mem.read_word(8), 0xCCCCCCCC);
}

TEST(MemoryTest, WriteWordZero) {
  Memory mem(1024);
  mem.write_word(0, 0xFFFFFFFF);
  mem.write_word(0, 0x00000000);
  EXPECT_EQ(mem.read_word(0), 0x00000000);
}

// Byte Operations Tests
TEST(MemoryTest, WriteAndReadByte) {
  Memory mem(1024);
  mem.write_byte(0, 0xAB);
  EXPECT_EQ(mem.read_byte(0), 0xAB);
}

TEST(MemoryTest, WriteMultipleBytes) {
  Memory mem(1024);
  mem.write_byte(0, 0x11);
  mem.write_byte(1, 0x22);
  mem.write_byte(2, 0x33);
  mem.write_byte(3, 0x44);
  
  EXPECT_EQ(mem.read_byte(0), 0x11);
  EXPECT_EQ(mem.read_byte(1), 0x22);
  EXPECT_EQ(mem.read_byte(2), 0x33);
  EXPECT_EQ(mem.read_byte(3), 0x44);
}

TEST(MemoryTest, ByteWriteDoesNotAffectOtherBytes) {
  Memory mem(1024);
  mem.write_word(0, 0xFFFFFFFF);
  mem.write_byte(1, 0x00);
  
  EXPECT_EQ(mem.read_byte(0), 0xFF);
  EXPECT_EQ(mem.read_byte(1), 0x00);
  EXPECT_EQ(mem.read_byte(2), 0xFF);
  EXPECT_EQ(mem.read_byte(3), 0xFF);
}

// Program Loading Tests
TEST(MemoryTest, LoadProgram) {
  Memory mem(1024);
  std::vector<word_t> program = {0x12345678, 0xABCDEF00, 0x11223344};
  mem.load_program(program, 0);
  
  EXPECT_EQ(mem.read_word(0), 0x12345678);
  EXPECT_EQ(mem.read_word(4), 0xABCDEF00);
  EXPECT_EQ(mem.read_word(8), 0x11223344);
}

TEST(MemoryTest, LoadProgramAtOffset) {
  Memory mem(1024);
  std::vector<word_t> program = {0xDEADBEEF, 0xCAFEBABE};
  mem.load_program(program, 100);
  
  EXPECT_EQ(mem.read_word(100), 0xDEADBEEF);
  EXPECT_EQ(mem.read_word(104), 0xCAFEBABE);
}

TEST(MemoryTest, LoadEmptyProgram) {
  Memory mem(1024);
  std::vector<word_t> program = {};
  mem.load_program(program, 0);
  // Should not crash
  EXPECT_EQ(mem.size(), 1024);
}

// Reset Tests
TEST(MemoryTest, ResetClearsMemory) {
  Memory mem(1024);
  mem.write_word(0, 0xFFFFFFFF);
  mem.write_word(4, 0xAAAAAAAA);
  mem.write_byte(100, 0xFF);
  
  mem.reset();
  
  EXPECT_EQ(mem.read_word(0), 0x00000000);
  EXPECT_EQ(mem.read_word(4), 0x00000000);
  EXPECT_EQ(mem.read_byte(100), 0x00);
}

TEST(MemoryTest, ResetPreservesSize) {
  Memory mem(2048);
  mem.reset();
  EXPECT_EQ(mem.size(), 2048);
}

// Initialization Tests
TEST(MemoryTest, DefaultInitialization) {
  Memory mem(1024);
  // All memory should be initialized to 0
  EXPECT_EQ(mem.read_word(0), 0);
  EXPECT_EQ(mem.read_word(4), 0);
  EXPECT_EQ(mem.read_byte(100), 0);
}

TEST(MemoryTest, CustomSize) {
  Memory mem(2048);
  EXPECT_EQ(mem.size(), 2048);
}

TEST(MemoryTest, DefaultSize) {
  Memory mem;
  EXPECT_EQ(mem.size(), 1024 * 1024);  // 1MB default
}

// Edge Cases
TEST(MemoryTest, WriteAtMaxAddress) {
  Memory mem(1024);
  mem.write_word(1020, 0x12345678);  // Last valid word-aligned address
  EXPECT_EQ(mem.read_word(1020), 0x12345678);
}

TEST(MemoryTest, WriteByteAtLastAddress) {
  Memory mem(1024);
  mem.write_byte(1023, 0xFF);  // Last byte
  EXPECT_EQ(mem.read_byte(1023), 0xFF);
}

TEST(MemoryTest, AlignedAccess) {
  Memory mem(1024);
  // Word accesses at aligned addresses
  mem.write_word(0, 0x11111111);
  mem.write_word(4, 0x22222222);
  mem.write_word(8, 0x33333333);
  
  EXPECT_EQ(mem.read_word(0), 0x11111111);
  EXPECT_EQ(mem.read_word(4), 0x22222222);
  EXPECT_EQ(mem.read_word(8), 0x33333333);
}

TEST(MemoryTest, OverwriteData) {
  Memory mem(1024);
  mem.write_word(0, 0xFFFFFFFF);
  mem.write_word(0, 0x12345678);
  EXPECT_EQ(mem.read_word(0), 0x12345678);
}

TEST(MemoryTest, NonZeroInitialValue) {
  Memory mem(1024);
  mem.write_word(500, 0xDEADBEEF);
  EXPECT_EQ(mem.read_word(500), 0xDEADBEEF);
  // Adjacent memory should still be zero
  EXPECT_EQ(mem.read_word(496), 0x00000000);
  EXPECT_EQ(mem.read_word(504), 0x00000000);
}
