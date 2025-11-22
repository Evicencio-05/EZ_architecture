#include "cli/command_parser.hpp"

#include <gtest/gtest.h>

using namespace ez_arch;

// Basic Command Parsing Tests
TEST(CommandParserTest, ParseHelp) {
  Command cmd = CommandParser::parse("help");
  EXPECT_EQ(cmd.type, CommandType::HELP);
  EXPECT_TRUE(cmd.args.empty());
}

TEST(CommandParserTest, ParseHelpShortForm) {
  Command cmd1 = CommandParser::parse("h");
  EXPECT_EQ(cmd1.type, CommandType::HELP);

  Command cmd2 = CommandParser::parse("?");
  EXPECT_EQ(cmd2.type, CommandType::HELP);
}

TEST(CommandParserTest, ParseLoad) {
  Command cmd = CommandParser::parse("load program.hex");
  EXPECT_EQ(cmd.type, CommandType::LOAD);
  ASSERT_EQ(cmd.args.size(), 1);
  EXPECT_EQ(cmd.args[0], "program.hex");
}

TEST(CommandParserTest, ParseLoadWithPath) {
  Command cmd = CommandParser::parse("load /path/to/file.hex");
  EXPECT_EQ(cmd.type, CommandType::LOAD);
  ASSERT_EQ(cmd.args.size(), 1);
  EXPECT_EQ(cmd.args[0], "/path/to/file.hex");
}

TEST(CommandParserTest, ParseStep) {
  Command cmd = CommandParser::parse("step");
  EXPECT_EQ(cmd.type, CommandType::STEP);
  EXPECT_TRUE(cmd.args.empty());
}

TEST(CommandParserTest, ParseStepShortForm) {
  Command cmd = CommandParser::parse("s");
  EXPECT_EQ(cmd.type, CommandType::STEP);
}

TEST(CommandParserTest, ParseStepWithCount) {
  Command cmd = CommandParser::parse("step 5");
  EXPECT_EQ(cmd.type, CommandType::STEP);
  ASSERT_EQ(cmd.args.size(), 1);
  EXPECT_EQ(cmd.args[0], "5");
}

TEST(CommandParserTest, ParseStepStage) {
  Command cmd = CommandParser::parse("stage");
  EXPECT_EQ(cmd.type, CommandType::STEP_STAGE);
}

TEST(CommandParserTest, ParseStepStageShortForm) {
  Command cmd = CommandParser::parse("st");
  EXPECT_EQ(cmd.type, CommandType::STEP_STAGE);
}

TEST(CommandParserTest, ParseRun) {
  Command cmd = CommandParser::parse("run");
  EXPECT_EQ(cmd.type, CommandType::RUN);
}

TEST(CommandParserTest, ParseRunShortForm) {
  Command cmd = CommandParser::parse("r");
  EXPECT_EQ(cmd.type, CommandType::RUN);
}

TEST(CommandParserTest, ParseRegisters) {
  Command cmd = CommandParser::parse("registers");
  EXPECT_EQ(cmd.type, CommandType::REGISTERS);
}

TEST(CommandParserTest, ParseRegistersShortForm) {
  Command cmd = CommandParser::parse("regs");
  EXPECT_EQ(cmd.type, CommandType::REGISTERS);
}

TEST(CommandParserTest, ParseRegister) {
  Command cmd = CommandParser::parse("register 5");
  EXPECT_EQ(cmd.type, CommandType::REGISTER);
  ASSERT_EQ(cmd.args.size(), 1);
  EXPECT_EQ(cmd.args[0], "5");
}

TEST(CommandParserTest, ParseRegisterShortForm) {
  Command cmd = CommandParser::parse("reg 10");
  EXPECT_EQ(cmd.type, CommandType::REGISTER);
  ASSERT_EQ(cmd.args.size(), 1);
  EXPECT_EQ(cmd.args[0], "10");
}

TEST(CommandParserTest, ParseMemory) {
  Command cmd = CommandParser::parse("memory 0x1000");
  EXPECT_EQ(cmd.type, CommandType::MEMORY);
  ASSERT_EQ(cmd.args.size(), 1);
  EXPECT_EQ(cmd.args[0], "0x1000");
}

TEST(CommandParserTest, ParseMemoryShortForm) {
  Command cmd = CommandParser::parse("mem 0x1000");
  EXPECT_EQ(cmd.type, CommandType::MEMORY);
}

TEST(CommandParserTest, ParseMemoryRange) {
  Command cmd = CommandParser::parse("memory 0x1000 0x2000");
  EXPECT_EQ(cmd.type, CommandType::MEMORY);
  ASSERT_EQ(cmd.args.size(), 2);
  EXPECT_EQ(cmd.args[0], "0x1000");
  EXPECT_EQ(cmd.args[1], "0x2000");
}

TEST(CommandParserTest, ParsePC) {
  Command cmd = CommandParser::parse("pc");
  EXPECT_EQ(cmd.type, CommandType::PC);
}

TEST(CommandParserTest, ParseReset) {
  Command cmd = CommandParser::parse("reset");
  EXPECT_EQ(cmd.type, CommandType::RESET);
}

TEST(CommandParserTest, ParseQuit) {
  Command cmd = CommandParser::parse("quit");
  EXPECT_EQ(cmd.type, CommandType::QUIT);
}

TEST(CommandParserTest, ParseQuitShortForm) {
  Command cmd1 = CommandParser::parse("q");
  EXPECT_EQ(cmd1.type, CommandType::QUIT);

  Command cmd2 = CommandParser::parse("exit");
  EXPECT_EQ(cmd2.type, CommandType::QUIT);
}

TEST(CommandParserTest, ParseUnknownCommand) {
  Command cmd = CommandParser::parse("invalid");
  EXPECT_EQ(cmd.type, CommandType::UNKNOWN);
}

// Case Insensitivity Tests
TEST(CommandParserTest, CaseInsensitiveHelp) {
  Command cmd1 = CommandParser::parse("HELP");
  EXPECT_EQ(cmd1.type, CommandType::HELP);

  Command cmd2 = CommandParser::parse("HeLp");
  EXPECT_EQ(cmd2.type, CommandType::HELP);

  Command cmd3 = CommandParser::parse("help");
  EXPECT_EQ(cmd3.type, CommandType::HELP);
}

TEST(CommandParserTest, CaseInsensitiveLoad) {
  Command cmd = CommandParser::parse("LOAD file.hex");
  EXPECT_EQ(cmd.type, CommandType::LOAD);
  ASSERT_EQ(cmd.args.size(), 1);
  EXPECT_EQ(cmd.args[0], "file.hex");
}

TEST(CommandParserTest, CaseInsensitiveStep) {
  Command cmd = CommandParser::parse("STEP");
  EXPECT_EQ(cmd.type, CommandType::STEP);
}

// Edge Cases
TEST(CommandParserTest, EmptyInput) {
  Command cmd = CommandParser::parse("");
  EXPECT_EQ(cmd.type, CommandType::UNKNOWN);
}

TEST(CommandParserTest, WhitespaceOnly) {
  Command cmd = CommandParser::parse("   ");
  EXPECT_EQ(cmd.type, CommandType::UNKNOWN);
}

TEST(CommandParserTest, MultipleSpaces) {
  Command cmd = CommandParser::parse("step    5");
  EXPECT_EQ(cmd.type, CommandType::STEP);
  ASSERT_EQ(cmd.args.size(), 1);
  EXPECT_EQ(cmd.args[0], "5");
}

TEST(CommandParserTest, LeadingSpaces) {
  Command cmd = CommandParser::parse("   help");
  EXPECT_EQ(cmd.type, CommandType::HELP);
}

TEST(CommandParserTest, TrailingSpaces) {
  Command cmd = CommandParser::parse("help   ");
  EXPECT_EQ(cmd.type, CommandType::HELP);
}

TEST(CommandParserTest, MultipleArguments) {
  Command cmd = CommandParser::parse("memory 0x1000 0x2000 0x3000");
  EXPECT_EQ(cmd.type, CommandType::MEMORY);
  ASSERT_EQ(cmd.args.size(), 3);
  EXPECT_EQ(cmd.args[0], "0x1000");
  EXPECT_EQ(cmd.args[1], "0x2000");
  EXPECT_EQ(cmd.args[2], "0x3000");
}

// Argument Preservation Tests
TEST(CommandParserTest, ArgumentsPreserveMixedCase) {
  Command cmd = CommandParser::parse("load MyProgram.HEX");
  EXPECT_EQ(cmd.type, CommandType::LOAD);
  ASSERT_EQ(cmd.args.size(), 1);
  // Args should preserve their original case
  EXPECT_EQ(cmd.args[0], "MyProgram.HEX");
}

TEST(CommandParserTest, HexAddressArgument) {
  Command cmd = CommandParser::parse("memory 0xABCD1234");
  EXPECT_EQ(cmd.type, CommandType::MEMORY);
  ASSERT_EQ(cmd.args.size(), 1);
  EXPECT_EQ(cmd.args[0], "0xABCD1234");
}

TEST(CommandParserTest, NegativeNumberArgument) {
  Command cmd = CommandParser::parse("step -1");
  EXPECT_EQ(cmd.type, CommandType::STEP);
  ASSERT_EQ(cmd.args.size(), 1);
  EXPECT_EQ(cmd.args[0], "-1");
}

// Special Character Tests
TEST(CommandParserTest, PathWithSlashes) {
  Command cmd = CommandParser::parse("load /home/user/program.hex");
  EXPECT_EQ(cmd.type, CommandType::LOAD);
  ASSERT_EQ(cmd.args.size(), 1);
  EXPECT_EQ(cmd.args[0], "/home/user/program.hex");
}

TEST(CommandParserTest, FileWithDots) {
  Command cmd = CommandParser::parse("load test.program.v2.hex");
  EXPECT_EQ(cmd.type, CommandType::LOAD);
  ASSERT_EQ(cmd.args.size(), 1);
  EXPECT_EQ(cmd.args[0], "test.program.v2.hex");
}
