#pragma once

#include <string>

namespace ez_arch {

/**
 * InputHandler provides a cross-platform command-line input interface.
 *
 * When USE_LINENOISE is defined, uses linenoise for:
 * - Command history (up/down arrows)
 * - Line editing (left/right arrows, backspace, etc.)
 *
 * Otherwise, falls back to standard std::getline.
 */
class InputHandler {
public:
  InputHandler();
  ~InputHandler() = default;

  static std::string readline(const char* prompt);

  static void addHistory(const std::string& line);

  static bool saveHistory(const char* filename);

  static bool loadHistory(const char* filename);

private:
  bool using_linenoise_;
};

} // namespace ez_arch
