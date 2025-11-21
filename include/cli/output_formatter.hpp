#pragma once

#include "core/cpu.hpp"
#include "core/memory.hpp"
#include "core/register_file.hpp"

#include <optional>

namespace ez_arch {

class OutputFormatter {
public:
  static void printRegisters(const RegisterFile& regs,
                             std::optional<register_id_t> reg = std::nullopt);
  static void printMemory(const Memory& mem, address_t start, address_t end);
  static void printCpuState(const CPU& cpu);
};
} // namespace ez_arch
