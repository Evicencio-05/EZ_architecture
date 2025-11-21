#include <cstdint>
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "cli/command_parser.hpp"
#include "cli/input_handler.hpp"
#include "cli/output_formatter.hpp"
#include "core/cpu.hpp"
#include "core/decoder.hpp"

using namespace ez_arch;

struct WatchExpression {
  std::string expr;
  enum Type : uint8_t { REGISTER, MEMORY } type;
  uint32_t value;
};

std::vector<WatchExpression> g_watches;

void printHelp();
std::vector<word_t> loadHexFile(const std::string& filename);
void printWatches(const CPU& cpu);

int main() { // NOLINT
  CPU cpu;
  bool running = true;
  InputHandler inputHandler;

  // Load command history from previous sessions
  InputHandler::load_history(".ez_arch_history");

  std::cout << "\nEZ architecture MIPS simulator\n\n"
            << "Type 'help' for commands\n\n";

  while (running) {
    std::string input = InputHandler::readline("> ");

    // Handle EOF (Ctrl+D)
    if (input.empty() && std::cin.eof()) {
      std::cout << "\n";
      break;
    }

    if (input.empty()) { continue;
}

    // Add non-empty commands to history
    InputHandler::add_history(input);

    Command cmd = CommandParser::parse(input);

    switch (cmd.type) {
      case CommandType::HELP:
        printHelp();
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
        if (!g_watches.empty()) {
          print_watches(cpu);
        }
        break;
      }

      case CommandType::STEP_STAGE:
        cpu.step_stage();
        OutputFormatter::print_cpu_state(cpu);
        if (!g_watches.empty()) {
          print_watches(cpu);
        }
        break;

      case CommandType::RUN:
        cpu.run();
        std::cout << "Execution halted\n";
        OutputFormatter::print_cpu_state(cpu);
        if (!g_watches.empty()) {
          print_watches(cpu);
        }
        break;

      case CommandType::REGISTERS:
        OutputFormatter::print_registers(cpu.get_registers());
        break;

      case CommandType::REGISTER:
        if (cmd.args.empty()) {
          std::cout << "Usage: reg <register_number>\n";
        } else {
          try {
            register_id_t regNum = std::stoi(cmd.args[0]);
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

      case CommandType::DISASSEMBLE: {
        try {
          address_t addr = cpu.get_registers().get_pc();
          if (!cmd.args.empty()) {
            addr = std::stoul(cmd.args[0], nullptr, 16);
          }
          word_t instruction = cpu.get_memory().read_word(addr);
          std::string assembly = Decoder::decode(instruction);
          std::cout << "0x" << std::hex << std::setw(8) << std::setfill('0')
                    << addr << ": 0x" << std::setw(8) << instruction << std::dec
                    << "  " << assembly << '\n';
        } catch (const std::exception& e) {
          std::cerr << "Error: " << e.what() << '\n';
        }
        break;
      }

      case CommandType::ASSEMBLE: {
        if (cmd.args.empty()) {
          std::cout << "Usage: asm <assembly instruction>\n";
          std::cout << "Example: asm add $t0, $t1, $t2\n";
        } else {
          try {
            // Reconstruct the full assembly line from args
            std::string assemblyLine = cmd.args[0];
            for (size_t i = 1; i < cmd.args.size(); ++i) {
              assemblyLine += " " + cmd.args[i];
            }

            word_t instruction = Decoder::assemble(assembly_line);
            std::cout << "0x" << std::hex << std::setw(8) << std::setfill('0')
                      << instruction << std::dec << '\n';
          } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << '\n';
          }
        }
        break;
      }

      case CommandType::WATCH: {
        if (cmd.args.empty()) {
          if (g_watches.empty()) {
            std::cout << "No watch expressions\n";
          } else {
            print_watches(cpu);
          }
        } else {
          const std::string& expr = cmd.args[0];
          WatchExpression watch;
          watch.expr = expr;

          // Parse watch expression
          if (expr[0] == '$') {
            // Register watch: $0, $1, etc.
            try {
              watch.type = WatchExpression::REGISTER;
              watch.value = std::stoi(expr.substr(1));
              if (watch.value >= 32) {
                std::cerr << "Invalid register number\n";
                break;
              }
              g_watches.push_back(watch);
              std::cout << "Added watch: " << expr << '\n';
            } catch (const std::exception& e) {
              std::cerr << "Invalid register format\n";
            }
          } else if (expr[0] == '0' && expr[1] == 'x') {
            // Memory watch: 0x1000, etc.
            try {
              watch.type = WatchExpression::MEMORY;
              watch.value = std::stoul(expr, nullptr, 16);
              g_watches.push_back(watch);
              std::cout << "Added watch: " << expr << '\n';
            } catch (const std::exception& e) {
              std::cerr << "Invalid address format\n";
            }
          } else {
            std::cerr << "Watch format: $<reg_num> or 0x<address>\n";
          }
        }
        break;
      }

      case CommandType::SAVE: {
        if (cmd.args.empty()) {
          std::cout << "Usage: save <filename>\n";
        } else {
          std::ofstream outfile(cmd.args[0], std::ios::binary);
          if (!outfile) {
            std::cerr << "Error: Could not open file for writing\n";
            break;
          }

          // Save registers (32 registers)
          for (register_id_t i = 0; i < 32; ++i) {
            word_t value = cpu.get_registers().read(i);
            outfile.write(reinterpret_cast<const char*>(&value), sizeof(value));
          }

          // Save PC
          word_t pc = cpu.get_registers().get_pc();
          outfile.write(reinterpret_cast<const char*>(&pc), sizeof(pc));

          // Save memory (iterate through memory and save non-zero words)
          // Simple approach: save first 64KB (0x0 to 0x10000)
          for (address_t addr = 0; addr < 0x10000; addr += 4) {
            try {
              word_t value = cpu.get_memory().read_word(addr);
              outfile.write(reinterpret_cast<const char*>(&addr), sizeof(addr));
              outfile.write(reinterpret_cast<const char*>(&value),
                            sizeof(value));
            } catch (...) {
              // Skip unreadable addresses
            }
          }

          std::cout << "CPU state saved to " << cmd.args[0] << '\n';
        }
        break;
      }

      case CommandType::LOAD_STATE: {
        if (cmd.args.empty()) {
          std::cout << "Usage: loadstate <filename>\n";
        } else {
          std::ifstream infile(cmd.args[0], std::ios::binary);
          if (!infile) {
            std::cerr << "Error: Could not open file for reading\n";
            break;
          }

          cpu.reset();

          // Load registers
          for (register_id_t i = 0; i < 32; ++i) {
            word_t value;
            infile.read(reinterpret_cast<char*>(&value), sizeof(value));
            if (i != 0) {  // Don't write to $0
              cpu.get_registers().write(i, value);
            }
          }

          // Load PC
          word_t pc;
          infile.read(reinterpret_cast<char*>(&pc), sizeof(pc));
          cpu.get_registers().set_pc(pc);

          // Load memory
          while (infile) {
            address_t addr;
            word_t value;
            infile.read(reinterpret_cast<char*>(&addr), sizeof(addr));
            if (!infile) { break;
}
            infile.read(reinterpret_cast<char*>(&value), sizeof(value));
            if (!infile) { break;
}
            try {
              cpu.get_memory().write_word(addr, value);
            } catch (...) {
              // Skip unwriteable addresses
            }
          }

          std::cout << "CPU state loaded from " << cmd.args[0] << '\n';
        }
        break;
      }

      case CommandType::RESET:
        cpu.reset();
        std::cout << "CPU reset\n";
        break;

      case CommandType::QUIT:
        InputHandler::save_history(".ez_arch_history");
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

void printHelp() {
  std::cout
      << "Available commands:\n"
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
      << "  disasm [addr]         - Disassemble instruction (default: at PC)\n"
      << "  asm <instruction>     - Assemble instruction to machine code\n"
      << "  watch <expr>          - Add watch expression ($reg or 0xaddr)\n"
      << "  save <file>           - Save CPU state to file\n"
      << "  loadstate <file>      - Load CPU state from file\n"
      << "  reset                 - Reset CPU state\n"
      << "  quit                  - Exit simulator\n";
}

std::vector<word_t> loadHexFile(const std::string& filename) {
  std::vector<word_t> program;
  std::ifstream file(filename);

  if (!file.is_open()) {
    std::cerr << "Error: Could not open file '" << filename << "'\n\n";
    return program;
  }

  std::string line;
  while (std::getline(file, line)) {
    if (line.empty() || line[0] == '#') { continue;
}

    try {
      word_t instruction = std::stoul(line, nullptr, 16);
      program.push_back(instruction);
    } catch (const std::exception& e) {
      std::cerr << "Error parsing line: " << line << '\n';
    }
  }

  return program;
}

void printWatches(const CPU& cpu) {
  std::cout << "\nWatch expressions:\n";
  for (const auto& watch : g_watches) {
    std::cout << "  " << watch.expr << " = ";

    try {
      if (watch.type == WatchExpression::REGISTER) {
        word_t value = cpu.get_registers().read(watch.value);
        std::cout << "0x" << std::hex << std::setw(8) << std::setfill('0')
                  << value << std::dec << " (" << static_cast<int32_t>(value)
                  << ")";
      } else {  // MEMORY
        word_t value = cpu.get_memory().read_word(watch.value);
        std::cout << "0x" << std::hex << std::setw(8) << std::setfill('0')
                  << value << std::dec;
      }
    } catch (const std::exception& e) {
      std::cout << "<error: " << e.what() << ">";
    }

    std::cout << '\n';
  }
  std::cout << '\n';
}
