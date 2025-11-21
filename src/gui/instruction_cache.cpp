#include "gui/instruction_cache.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace ez_arch {

static std::string getHome() {
  const char* h = std::getenv("HOME");
  if ((h != nullptr) && (*h != 0)) { return std::string(h); }
  return "."; // fallback
}

std::string InstructionCache::cachePath() {
  // ~/.cache/ez_arch/instructions.asm
  std::string base = getHome() + "/.cache/ez_arch";
  fs::create_directories(base);
  return base + "/instructions.asm";
}

std::vector<std::string> InstructionCache::load() {
  std::vector<std::string> lines;
  std::ifstream in(cachePath());
  if (!in.is_open()) { return lines; }
  std::string line;
  while (std::getline(in, line)) {
    if (!line.empty()) { lines.push_back(line); }
  }
  return lines;
}

void InstructionCache::save(const std::vector<std::string>& lines) {
  std::ofstream out(cachePath(), std::ios::trunc);
  for (const auto& l : lines) {
    out << l << "\n";
  }
}

void InstructionCache::append(const std::string& line) {
  std::ofstream out(cachePath(), std::ios::app);
  out << line << "\n";
}

void InstructionCache::clear() {
  std::ofstream out(cachePath(), std::ios::trunc);
  (void)out;
}

} // namespace ez_arch
