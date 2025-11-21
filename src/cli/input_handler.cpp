// NOLINTBEGIN

#include "cli/input_handler.hpp"

#ifdef USE_LINENOISE
extern "C" {
#  include "../../external/linenoise.h"
}
#endif

namespace ez_arch {

InputHandler::InputHandler() {
#ifdef USE_LINENOISE
  using_linenoise_ = true;
#else
  using_linenoise_ = false;
#endif
}

std::string InputHandler::readline(const char* prompt) {
#ifdef USE_LINENOISE
  char* line = linenoise(prompt);
  if (line == nullptr) {
    return ""; // EOF or error
  }
  std::string result(line);
  free(line); // linenoise allocates with malloc
  return result;
#else
  std::cout << prompt;
  std::string line;
  if (!std::getline(std::cin, line)) {
    return ""; // EOF or error
  }
  return line;
#endif
}

void InputHandler::addHistory(const std::string& line) {
#ifdef USE_LINENOISE
  if (!line.empty()) { linenoiseHistoryAdd(line.c_str()); }
#else
  (void)line; // Suppress unused parameter warning
#endif
}

bool InputHandler::saveHistory(const char* filename) {
#ifdef USE_LINENOISE
  return linenoiseHistorySave(filename) == 0;
#else
  (void)filename;
  return false;
#endif
}

bool InputHandler::loadHistory(const char* filename) {
#ifdef USE_LINENOISE
  return linenoiseHistoryLoad(filename) == 0;
#else
  (void)filename;
  return false;
#endif
}

} // namespace ez_arch

// NOLINTEND
