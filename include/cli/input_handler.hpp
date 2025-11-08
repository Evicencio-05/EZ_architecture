#ifndef INPUT_HANDLER_HPP
#define INPUT_HANDLER_HPP

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
  ~InputHandler();

  /**
   * Read a line of input from the user with the given prompt.
   * Returns empty string on EOF or error.
   */
  std::string readline(const char* prompt);

  /**
   * Add a line to the command history.
   * Only effective when linenoise is enabled.
   */
  void add_history(const std::string& line);

  /**
   * Save command history to a file.
   * Only effective when linenoise is enabled.
   */
  bool save_history(const char* filename);

  /**
   * Load command history from a file.
   * Only effective when linenoise is enabled.
   */
  bool load_history(const char* filename);

private:
  bool using_linenoise_;
};

} // namespace ez_arch

#endif // INPUT_HANDLER_HPP
