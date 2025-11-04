#include <gtest/gtest.h>
#include "core/memory.hpp"

using namespace ez_arch;

  TEST(MemoryTest, WriteAndRead) {
    Memory mem(1024);
    mem.write_word(0, 0x12345678);
    EXPECT_EQ(mem.read_byte(0), 0x12);  // Big-endian test
    EXPECT_EQ(mem.read_word(0), 0x12345678);
}
