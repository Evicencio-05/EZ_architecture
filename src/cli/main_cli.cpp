#include "core/cpu.hpp"
#include "core/decoder.hpp"
#include "cli/command_parser.hpp"
#include "cli/output_formatter.hpp"
#include <iostream>
#include <string>

using namespace ez_arch;

int main() {
  CPU cpu;
  bool running = true;

  std::cout << "EZ architecture MIPS simulator\n"
    << "Type 'help' for commands\n\n";

  while (running) {
    std::cout << "> ";
    std::string input;
    std::getline(cin, input);

    if (input.empty()) continue;

    Command cmd = CommandParser::parse(input);

    switch (cmd.type) {
      case CommandType::HELP;
        print_help();
        break;

      case CommandType::LOAD:
        // Load program from file
        break;

      case CommandType::STEP:
        cpu.step();
        OutputFormatter::print_cpu_state(cpu);
        break;
       
      case CommandType::RUN:
        cpu.run();
        std::cout << "Execution halted\n";
        OutputFormatter::print_cpu_state();

      case CommandType::REGISTERS:
        OutputFormatter::print_registers(cpu.get_registers());
        break;

      case CommandType::MEMORY:
        // Print memory range
        break;

      case CommandType::RESET:
        cpu.reset();
        std::cout << "CPU reset\n";
        break;

      case CommandType::QUIT;
        running = false;
        break;

      case CommandType::UNKNOWN:
        std::cout << "Unknown command. Type 'help' for available commands\n";
    }
  }

  return 0;
}

void print_help() {
  std::cout << "Available commands:\n"
            << "  help                  - Show this help\n"
            << "  load <file>           - Load program from file\n"
            << "  step                  - Execute one instruction\n"
            << "  step <n>              - Execute n instructions\n"
            << "  run                   - Run until halt\n"
            << "  registers             - Display all registers\n"
            << "  reg <num>             - Display specific register\n"
            << "  memory                - Display memory at address\n"
            << "  memory <start> <end>  - Display memory in range\n"
            << "  pc                    - Display program counter\n"
            << "  reset                 - Reset CPU state\n"
            << "  quit                  - Exit simulator\n";
}
