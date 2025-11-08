#include "core/cpu.hpp"
#include <iostream>
#include <iomanip>

using namespace ez_arch;

// Helper to make I-type instruction
word_t make_i_instruction(uint8_t opcode, uint8_t rs, uint8_t rt, int16_t imm) {
    return (static_cast<word_t>(opcode) << 26) |
           (static_cast<word_t>(rs) << 21) |
           (static_cast<word_t>(rt) << 16) |
           (static_cast<word_t>(imm) & 0xFFFF);
}

int main() {
    CPU cpu;
    std::vector<word_t> program = {make_i_instruction(0x04, 1, 2, 4)};  // BEQ
    
    cpu.load_program(program);
    cpu.get_registers().write(1, 42);
    cpu.get_registers().write(2, 42);
    
    std::cout << "Before step:\n";
    std::cout << "  r1 = " << cpu.get_registers().read(1) << "\n";
    std::cout << "  r2 = " << cpu.get_registers().read(2) << "\n";
    std::cout << "  PC = " << cpu.get_registers().get_pc() << "\n\n";
    
    cpu.step();
    
    std::cout << "After step:\n";
    std::cout << "  PC = " << cpu.get_registers().get_pc() << "\n";
    std::cout << "  Expected: 20\n";
    std::cout << "  Got: " << cpu.get_registers().get_pc() << "\n";
    
    return 0;
}
