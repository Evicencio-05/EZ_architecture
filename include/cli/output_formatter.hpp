# pragma once 

#include "core/cpu.hpp"
#include "core/register_file.hpp"
#include "core/memory.hpp"
#include <string>
#include <optional>

namespace ez_arch {

  class OutputFormatter {
  public:
    static void print_registers(const RegisterFile& regs,
        std::optional<register_id_t> reg = std::nullopt);
    static void print_memory(const Memory& mem, address_t start, address_t end);
    static void print_cpu_state(const CPU& cpu);
  };
} // ez_arch
