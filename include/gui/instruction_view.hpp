#pragma once

#include <SFML/Graphics.hpp>
#include "core/memory.hpp"
#include "core/register_file.hpp"
#include "core/decoder.hpp"
#include <string>

namespace ez_arch {

class InstructionView {
public:
    InstructionView(const Memory& memory, const RegisterFile& registers, sf::Font& font);
    
    // Set position of the view on screen
    void set_position(float x, float y);
    
    // Update state
    void update();
    
    // Draw the instruction view
    void draw(sf::RenderWindow& window);
    
    // Set which instruction range to display (start address and count)
    void set_display_range(address_t start_addr, size_t num_instructions);
    
private:
    const Memory& m_memory;
    const RegisterFile& m_registers;  // To get PC for highlighting
    sf::Font& m_font;
    float m_x{0.F};
    float m_y{0.F};
    
    // Display settings
    address_t m_startAddr{0};
    size_t m_numInstructions{16};
    
    // Drawing helpers
    void draw_header(sf::RenderWindow& window);
    void draw_instruction(sf::RenderWindow& window, address_t addr, float row_y, bool is_pc);
    
    // Instruction decoding helper
    [[nodiscard]] static std::string decode_instruction(word_t instruction) ;
};

} // namespace ez_arch
