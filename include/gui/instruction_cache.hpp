#pragma once

#include <string>
#include <vector>

namespace ez_arch {

// Simple persistence for instruction assembly lines
class InstructionCache {
public:
  // Load cached instructions from disk. Returns list of assembly lines.
  static std::vector<std::string> load();

  // Save the provided list to disk, overwriting existing cache.
  static void save(const std::vector<std::string>& lines);

  // Append a single instruction to the cache and flush to disk.
  static void append(const std::string& line);

  // Clear cache on disk.
  static void clear();

  // Path used for cache file
  static std::string cachePath();
};

} // namespace ez_arch
