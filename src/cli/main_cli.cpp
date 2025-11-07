#include "core/cpu.hpp"
#include "core/decoder.hpp"
#include "cli/command_parser.hpp"
#include "cli/output_formatter.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <exception>

using namespace ez_arch;

void print_help();
std::vector<word_t> load_hex_file(const std::string& filename);

int main() {
  CPU cpu;
  bool running = true;

  std::cout << "\nEZ architecture MIPS simulator\n\n"
    << "Type 'help' for commands\n\n";

  while (running) {
    std::cout << "> ";
    std::string input;
    std::getline(std::cin, input);

    if (input.empty()) continue;

    Command cmd = CommandParser::parse(input);

    switch (cmd.type) {
      case CommandType::HELP:
        print_help();
        break;

      case CommandType::LOAD:
        if (cmd.args.empty()) {
          std::cout << "Usage: load <filename>\n";
        } else {
          std::vector<word_t> program = load_hex_file(cmd.args[0]);
          if (!program.empty()) {
            cpu.load_program(program);
            std::cout << "Loaded " << program.size() << " instructions\n";
          }
        }
        break;

      case CommandType::STEP: {
        int count = 1;
        if (!cmd.args.empty()) {
          try {
            count = std::stoi(cmd.args[0]);
          } catch (const std::exception& e) {
            std::cerr << "Invalid step count.\n";
          }
        }

        for (int i = 0; i < count && !cpu.is_halted(); ++i) {
          cpu.step();
        }

        OutputFormatter::print_cpu_state(cpu);
        break;
      }

      case CommandType::STEP_STAGE:
        cpu.step_stage();
        OutputFormatter::print_cpu_state(cpu);
        break;

      case CommandType::RUN:
        cpu.run();
        std::cout << "Execution halted\n";
        OutputFormatter::print_cpu_state(cpu);
        break;

      case CommandType::REGISTERS:
        OutputFormatter::print_registers(cpu.get_registers());
        break;

      case CommandType::REGISTER:
        if (cmd.args.empty()) {
          std::cout << "Usage: reg <register_number>\n";
        } else {
          try {
            register_id_t reg_num = std::stoi(cmd.args[0]);
            OutputFormatter::print_registers(cpu.get_registers(), reg_num);
          } catch (const std::invalid_argument& e) {
            std::cerr << "Input could not be converted to register number.\n";
          } catch (const std::out_of_range& e) {
            std::cerr << "Invalid register number.\n";
          }
        }
        break;

      case CommandType::MEMORY:
        if (cmd.args.empty()) {
          std::cout << "Usage: memory <start_address> [end_address]\n";
        } else {
          try {
            address_t start = std::stoul(cmd.args[0], nullptr, 16);
            address_t end = (cmd.args.size() > 1)
              ? std::stoul(cmd.args[1], nullptr, 16)
              : start + 64;
            OutputFormatter::print_memory(cpu.get_memory(), start, end);
          } catch (const std::exception& e) {
            std::cerr << "Invalid address format.\n";
          }
        }
        break;

      case CommandType::PC: {
        const RegisterFile& regs = cpu.get_registers();
        std::cout << "PC: 0x" << std::hex << std::setw(8) << std::setfill('0')
                  << regs.get_pc() << std::dec << '\n';
        break;
      }

      case CommandType::RESET:
        cpu.reset();
        std::cout << "CPU reset\n";
        break;

      case CommandType::QUIT:
        running = false;
        break;

      case CommandType::UNKNOWN:
        std::cout << "Unknown command. Type 'help' for available commands\n";
        break;

      default:
        break;
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
            << "  stage                 - Execute by stage\n"
            << "  run                   - Run until halt\n"
            << "  registers             - Display all registers\n"
            << "  reg <num>             - Display specific register\n"
            << "  memory                - Display memory at address\n"
            << "  memory <start> <end>  - Display memory in range\n"
            << "  pc                    - Display program counter\n"
            << "  reset                 - Reset CPU state\n"
            << "  quit                  - Exit simulator\n";
}


std::vector<word_t> load_hex_file(const std::string& filename) {
  std::vector<word_t> program;
  std::ifstream file(filename);

  if (!file.is_open()) {
    std::cerr << "Error: Could not open file '" << filename << "'\n\n";
    return program;
  }

  std::string line;
  while (std::getline(file, line)) {
    if (line.empty() || line[0] == '#') continue;

    try {
      word_t instruction = std::stoul(line, nullptr, 16);
      program.push_back(instruction);
    } catch (const std::exception& e) {
      std::cerr << "Error parsing line: " << line << '\n';
    }
  }

  return program;
}
