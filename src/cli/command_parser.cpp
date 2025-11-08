#include "cli/command_parser.hpp"
#include <sstream>
#include <algorithm>

namespace ez_arch {

  Command CommandParser::parse(std::string_view input) {
    Command cmd;
    std::vector<std::string> tokens = tokenize(input);
    
    if (tokens.empty()) {
      cmd.type = CommandType::UNKNOWN;
      return cmd;
    }

    std::string command = tokens[0];

    std::transform(command.begin(), command.end(), command.begin(), ::tolower);
    
    if (tokens.size() > 1) {
      cmd.args.assign(tokens.begin() + 1, tokens.end());
    }

    if (command == "help" || command == "h" || command == "?") {
      cmd.type = CommandType::HELP;
    } else if (command == "load") {
      cmd.type = CommandType::LOAD;
    } else if (command == "step" || command == "s") {
      cmd.type = CommandType::STEP;
    } else if (command == "stage" || command == "st") {
      cmd.type = CommandType::STEP_STAGE;
    } else if (command == "run" || command == "r") {
      cmd.type = CommandType::RUN;
    } else if (command == "registers" || command == "regs") {
      cmd.type = CommandType::REGISTERS;
    } else if (command == "register" || command == "reg") {
      cmd.type = CommandType::REGISTER;
    } else if (command == "memory" || command == "mem") {
      cmd.type = CommandType::MEMORY;
    } else if (command == "pc") {
      cmd.type = CommandType::PC;
    } else if (command == "disasm" || command == "dis" || command == "d") {
      cmd.type = CommandType::DISASSEMBLE;
    } else if (command == "asm" || command == "a") {
      cmd.type = CommandType::ASSEMBLE;
    } else if (command == "watch" || command == "w") {
      cmd.type = CommandType::WATCH;
    } else if (command == "save") {
      cmd.type = CommandType::SAVE;
    } else if (command == "loadstate") {
      cmd.type = CommandType::LOAD_STATE;
    } else if (command == "reset") {
      cmd.type = CommandType::RESET;
    } else if (command == "quit" || command == "exit" || command == "q") {
      cmd.type = CommandType::QUIT;
    } else {
      cmd.type = CommandType::UNKNOWN;
    }
    
    return cmd;
  }

  std::vector<std::string> CommandParser::tokenize(std::string_view input) {
    std::vector<std::string> tokens;
    std::string input_str(input);
    std::istringstream iss(input_str);
    std::string token;

    while (iss >> token) {
      tokens.push_back(token);
    }

    return tokens;
  }

} // namespace ez_arch
