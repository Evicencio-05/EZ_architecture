# pragma once

#include <string>
#include <string_view>
#include <vector>

namespace ez_arch {

  enum class CommandType {
    HELP,
    LOAD,
    STEP,
    RUN,
    REGISTERS,
    REGISTER,
    MEMORY,
    PC,
    RESET,
    QUIT,
    UNKNOWN
  };

  stuct Command {
    CommandType type;
    std::vector<std::string> args;
  }

  class CommandParser {
    public: 
      static Command parse(std::string_view input);

    private:
      static std::vector<std::string> tokenize(std::string_view input);
  }

} // namespace ez_arch
