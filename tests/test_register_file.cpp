#include <gtest/gtest.h>
#include "core/register_file.hpp"

using namespace ez_arch;

TEST(RegisterFileTest, CreateRegisterFile) {
  RegisterFile regFileObj;

  for (register_id_t i = 0; i < RegisterFile::kNUM_REGISTERS; ++i) {
    EXPECT_EQ(regFileObj.read(i), 0);
  }
}

TEST(RegisterFileTest, WriteToValidRegister) {
  RegisterFile regFileObj;

  regFileObj.write(10, 1);
  ASSERT_EQ(regFileObj.read(10), 1);
}

TEST(RegisterFileTest, WriteToZeroRegister) {
  RegisterFile regFileObj;
  regFileObj.write(0, 1);

  ASSERT_EQ(regFileObj.read(0), 0);
}
