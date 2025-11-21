#include "gui/instruction_cache.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

namespace ez_arch {

static std::string getHome() {
#ifdef _WIN32
  // Windows: Try USERPROFILE first (standard), then HOMEPATH + HOMEDRIVE
  const char* profile = std::getenv("USERPROFILE");
  if (profile != nullptr && *profile != '\0') { return std::string(profile); }

  // Fallback for older Windows systems
  const char* home_drive = std::getenv("HOMEDRIVE");
  const char* home_path = std::getenv("HOMEPATH");
  if (home_drive != nullptr && home_path != nullptr && *home_drive != '\0'
      && *home_path != '\0') {
    return std::string(home_drive) + std::string(home_path);
  }
#else
  // Linux and macOS: Use HOME environment variable
  const char* home = std::getenv("HOME");
  if (home != nullptr && *home != '\0') { return std::string {home}; }
#endif

  // Fallback to current directory if all else fails
  return ".";
}

static std::string getCachePath() {
  std::string base = getHome();

#ifdef _WIN32
  // Windows: Use AppData/Local folder for cache
   // Creates: C:\Users\Username\AppData\Local\ez_arch\
  base += "\\AppData\\Local\\ez_arch";
#else
  // Linux and macOS: Use XDG_CACHE_HOME or default ~/.cache
  // Creates: ~/.cache/ez_arch/
  const char* xdgCache = std::getenv("XDG_CACHE_HOME");
  if (xdgCache != nullptr && *xdgCache != '\0') {
    base = std::string(xdgCache) + "/ez_arch";
  } else {
    base += "/.cache/ez_arch";
  }
#endif

  try {
    fs::create_directories(base);
  } catch (const fs::filesystem_error&) { return "ez_arch_instructions.asm"; }

  return base
         + (
#ifdef _WIN32
             "\\"
#else
             "/"
#endif
             "instructions.asm");
}

std::string InstructionCache::cachePath() {
  static std::string s_path = getCachePath();
  return s_path;
}

std::vector<std::string> InstructionCache::load() {
  std::vector<std::string> lines;
  std::string path = cachePath();

  try {
    std::ifstream in(path);
    if (!in.is_open()) { return lines; }

    std::string line;
    while (std::getline(in, line)) {
      if (!line.empty()) { lines.push_back(line); }
    }
  } catch (const std::exception&) { return lines; }

  return lines;
}

void InstructionCache::save(const std::vector<std::string>& lines) {
  std::string path = cachePath();

  try {
    std::ofstream out(path, std::ios::trunc);
    if (out.is_open()) {
      for (const auto& l : lines) {
        out << l << "\n";
      }
    }
  } catch (const std::exception&) {}
}

void InstructionCache::append(const std::string& line) {
  std::string path = cachePath();

  try {
    std::ofstream out(path, std::ios::app);
    if (out.is_open()) { out << line << "\n"; }
  } catch (const std::exception&) {}
}

void InstructionCache::clear() {
  std::string path = cachePath();

  try {
    std::ofstream out(path, std::ios::trunc);
  } catch (const std::exception&) {}
}

} // namespace ez_arch
