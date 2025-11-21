#include "cli/output_formatter.hpp"

#include "core/register_names.hpp"

#include <iomanip>
#include <iostream>


namespace ez_arch {

void OutputFormatter::printRegisters(const RegisterFile& regs,
                                     std::optional<register_id_t> reg) {
  if (!reg) {
    std::cout << "\nREGISTERS\n" << std::string(50, '-') << "\n";

    size_t registersSize = kREGISTER_NAMES.size();

    for (size_t i = 0; i < registersSize; i += 4) {
      for (size_t j = 0; j < 4 && (i + j) < registersSize; ++j) {
        std::cout << std::setw(5) << std::setfill(' ') << kREGISTER_NAMES[i + j]
                  << ": "
                  << "0x" << std::hex << std::setw(8) << std::setfill('0')
                  << regs.read(static_cast<int>(i + j)) << std::dec << "  ";
      }
      std::cout << '\n';
    }

    std::cout << "\nPC: 0x" << std::hex << std::setw(8) << std::setfill('0')
              << regs.getPc() << std::dec << '\n'
              << std::string(50, '-') << '\n';

  } else if (*reg > 31) {
    std::cout << "Register number must be 0-31\n";
  } else {
    std::cout << kREGISTER_NAMES[*reg] << " ($ " << static_cast<int>(*reg)
              << "): "
              << "0x" << std::hex << std::setw(8) << std::setfill('0')
              << regs.read(*reg) << std::dec << "(decimal: " << regs.read(*reg)
              << ")\n";
  }
}

void OutputFormatter::printMemory(const Memory& mem,
                                  address_t start,
                                  address_t end) {
  std::cout << "\nMEMORY [0x" << std::hex << start << " - 0x" << end << "]:\n"
            << std::string(50, '-') << "\n";

  for (address_t addr = start; addr < end; addr += 4) {
    if ((addr - start) % 16 == 0) {
      std::cout << "0x" << std::hex << std::setw(8) << std::setfill('0') << addr
                << ": ";
    }

    word_t word = mem.readWord(addr);
    std::cout << std::setw(8) << std::setfill('0') << word << " ";

    if ((addr - start + 4) % 16 == 0) { std::cout << '\n'; }
  }

  std::cout << std::dec << '\n' << std::string(50, '-') << '\n';
}

void OutputFormatter::printCpuState(const CPU& cpu) {
  const RegisterFile& regs = cpu.getRegisters();

  std::cout << "\n--- CPU STATE ---\n"
            << "PC: 0x" << std::hex << std::setw(8) << std::setfill('0')
            << regs.getPc() << std::dec << '\n'
            << stageToString(cpu.getCurrentStage()) << '\n'
            << "Halted: " << (cpu.isHalted() ? "Yes" : "No") << '\n'
            << std::string(17, '-') << '\n';
}

} // namespace ez_arch
