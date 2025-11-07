#include "cli/output_formatter.hpp"
#include "core/register_names.hpp"
#include <iostream>
#include <iomanip>


namespace ez_arch {
  
  void OutputFormatter::print_registers(const RegisterFile& regs, std::optional<register_id_t> reg) {
    if (!reg) {
      std::cout << "\nREGISTERS\n"
                << std::string(50, '-') << "\n";

      size_t registers_size = REGISTER_NAMES.size();

      for (size_t i = 0; i < registers_size; i += 4) {
        for (size_t j = 0; j < 4 && (i + j) < registers_size; ++j) {
          std::cout << std::setw(5) << std::setfill(' ') << REGISTER_NAMES[i + j] << ": "
                    << "0x" << std::hex << std::setw(8) << std::setfill('0')
                    << regs.read(static_cast<int>(i + j)) << std::dec << "  ";
        }
        std::cout << '\n';
      }

      std::cout << "\nPC: 0x" << std::hex << std::setw(8) << std::setfill('0')
                << regs.get_pc() << std::dec << '\n'
                << std::string(50, '-') << '\n';

    } else if (*reg > 31) {
      std::cout << "Register number must be 0-31\n";
    } else {
      std::cout << REGISTER_NAMES[*reg] << " ($ " << static_cast<int>(*reg) << "): "
                << "0x" << std::hex << std::setw(8) << std::setfill('0')
                << regs.read(*reg) << std::dec
                << "(decimal: " << regs.read(*reg) << ")\n";
    }
  }

  void OutputFormatter::print_memory(const Memory& mem, address_t start, address_t end) {
    std::cout << "\nMEMORY [0x" << std::hex << start << " - 0x" << end << "]:\n"
              << std::string(50, '-') << "\n";

    for (address_t addr = start; addr < end; addr += 4) {
      if ((addr - start) % 16 == 0) {
        std::cout << "0x" << std::hex << std::setw(8) << std::setfill('0')
                  << addr << ": ";
      }

      word_t word = mem.read_word(addr);
      std::cout << std::setw(8) << std::setfill('0') << word << " ";

      if ((addr - start + 4) % 16 == 0) {
        std::cout << '\n';
      }
    }

    std::cout << std::dec << '\n'
              << std::string(50, '-') << '\n';
  }

  void OutputFormatter::print_cpu_state(const CPU& cpu) {
    const RegisterFile& regs = cpu.get_registers();

    std::cout << "\n--- CPU STATE ---\n"
              << "PC: 0x" << std::hex << std::setw(8) << std::setfill('0')
              << regs.get_pc() << std::dec << '\n'
              << stageToString(cpu.get_current_stage()) << '\n'
              << "Halted: " << (cpu.is_halted() ? "Yes" : "No") << '\n'
              << std::string(17, '-') << '\n';
  }

} // namespace ez_arch
