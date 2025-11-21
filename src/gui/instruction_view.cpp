#include "gui/instruction_view.hpp"
#include "gui/style.hpp"
#include "core/decoder.hpp"
#include <iomanip>
#include <sstream>

namespace ez_arch {

InstructionView::InstructionView(const Memory& memory, const RegisterFile& registers, sf::Font& font)
    : m_memory(memory), m_registers(registers), m_font(font) {
    // Default: show first 16 instructions
}

void InstructionView::set_position(float x, float y) {
    m_x = x;
    m_y = y;
}

void InstructionView::set_display_range(address_t start_addr, size_t num_instructions) {
    m_startAddr = start_addr;
    m_numInstructions = num_instructions;
}

void InstructionView::update() {
    // Could auto-scroll to follow PC here in the future
}

void InstructionView::draw(sf::RenderWindow& window) {
    draw_header(window);
    
    // Get current PC for highlighting
    word_t current_pc = m_registers.get_pc();
    
    // Draw each instruction
    const float kHEADER_HEIGHT = 60.F;
    
    for (size_t i = 0; i < m_numInstructions; i++) {
        address_t addr = m_startAddr + (i * 4);  // 4 bytes per instruction
        float row_y = m_y + kHEADER_HEIGHT + (i * kROW_HEIGHT);
        
        // Check if this address is the current PC
        bool is_pc = (addr == current_pc);
        
        draw_instruction(window, addr, row_y, is_pc);
    }
}

void InstructionView::draw_header(sf::RenderWindow& window) {
    // Calculate background size based on number of instructions
    float height = 60.F + (m_numInstructions * kROW_HEIGHT) + kPADDING;
    
    // Draw background box
    sf::RectangleShape background({500.F, height});
    background.setPosition({m_x, m_y});
    background.setFillColor(kVIEW_BOX_BACKGROUND_COLOR);
    background.setOutlineColor(kVIEW_BOX_OUTLINE_COLOR);
    background.setOutlineThickness(kVIEW_BOX_OUTLINE_THICKNESS);
    window.draw(background);
    
    // Draw title
    sf::Text title(m_font);
    title.setString("Instructions");
    title.setCharacterSize(kTITLE_FONT_SIZE);
    title.setFillColor(kTITLE_TEXT_COLOR);
    title.setPosition({m_x + kPADDING, m_y + kPADDING});
    window.draw(title);
    
    // Draw column headers
    sf::Text header(m_font);
    header.setString("Address       Hex          Assembly");
    header.setCharacterSize(kHEADER_FONT_SIZE);
    header.setFillColor(kHEADER_TEXT_COLOR);
    header.setPosition({m_x + kPADDING, m_y + 40.F});
    window.draw(header);
}

void InstructionView::draw_instruction(sf::RenderWindow& window, address_t addr, float row_y, bool is_pc) {
    // Read the instruction word from memory
    word_t instruction_word = m_memory.read_word(addr);
    
    // If this is the PC location, draw a highlight background
    if (is_pc) {
        sf::RectangleShape highlight({480.F, kROW_HEIGHT});
        highlight.setPosition({m_x + kPADDING, row_y});
        highlight.setFillColor(kPC_HIGHLIGHT_COLOR);
        highlight.setOutlineColor(kPC_HIGHLIGHT_OUTLINE);
        highlight.setOutlineThickness(1.F);
        window.draw(highlight);
    }
    
    // Format address as hex
    std::ostringstream addr_stream;
    addr_stream << "0x" << std::hex << std::setw(8) << std::setfill('0') << addr;
    
    // Format instruction word as hex
    std::ostringstream hex_stream;
    hex_stream << "0x" << std::hex << std::setw(8) << std::setfill('0') << instruction_word;
    
    // Decode instruction to assembly
    std::string assembly = decode_instruction(instruction_word);
    
    // Draw address
    sf::Text addr_text(m_font);
    addr_text.setString(addr_stream.str());
    addr_text.setCharacterSize(kNORMAL_FONT_SIZE);
    addr_text.setFillColor(kNORMAL_TEXT_COLOR);
    addr_text.setPosition({m_x + kPADDING, row_y});
    window.draw(addr_text);
    
    // Draw hex encoding
    sf::Text hex_text(m_font);
    hex_text.setString(hex_stream.str());
    hex_text.setCharacterSize(kNORMAL_FONT_SIZE);
    hex_text.setFillColor(kHEADER_TEXT_COLOR);
    hex_text.setPosition({m_x + 120.F, row_y});
    window.draw(hex_text);
    
    // Draw assembly instruction
    sf::Text asm_text(m_font);
    asm_text.setString(assembly);
    asm_text.setCharacterSize(kNORMAL_FONT_SIZE);
    asm_text.setFillColor(is_pc ? sf::Color::Black : kVALUE_TEXT_COLOR);  // Black if PC for contrast
    asm_text.setPosition({m_x + 240.F, row_y});
    window.draw(asm_text);
}

std::string InstructionView::decode_instruction(word_t instruction) {
    // Handle NOP (all zeros) specially
    if (instruction == 0) {
        return "nop";
    }
    
    // Use the Decoder to decode the instruction
    try {
        return Decoder::decode(instruction);
    } catch (...) {
        // If decoding fails, return a generic format
        return "???";
    }
}

} // namespace ez_arch