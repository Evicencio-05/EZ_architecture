#include "gui/memory_view.hpp"
#include "gui/style.hpp"
#include <iomanip>
#include <sstream>

namespace ez_arch {

MemoryView::MemoryView(const Memory& memory, const RegisterFile& registers, sf::Font& font)
    : m_memory(memory), m_registers(registers), m_font(font) {
    // Default: show first 16 words of memory
}

void MemoryView::set_position(float x, float y) {
    m_x = x;
    m_y = y;
}

void MemoryView::set_display_range(address_t start_addr, size_t num_words) {
    m_startAddr = start_addr;
    m_numWords = num_words;
}

void MemoryView::update() {
    // Could auto-scroll to follow PC here in the future
}

void MemoryView::draw(sf::RenderWindow& window) {
    draw_header(window);
    
    // Get current PC for highlighting
    word_t current_pc = m_registers.get_pc();
    
    // Draw each memory word
    const float kHEADER_HEIGHT = 60.F;
    
    for (size_t i = 0; i < m_numWords; i++) {
        address_t addr = m_startAddr + (i * 4);  // 4 bytes per word
        float row_y = m_y + kHEADER_HEIGHT + (i * kROW_HEIGHT);
        
        // Check if this address is the current PC
        bool is_pc = (addr == current_pc);
        
        draw_memory_word(window, addr, row_y, is_pc);
    }
}

void MemoryView::draw_header(sf::RenderWindow& window) {
    // Calculate background size based on number of words to display
    float height = 60.F + (m_numWords * kROW_HEIGHT) + kPADDING;
    
    // Draw background box
    sf::RectangleShape background({350.F, height});
    background.setPosition({m_x, m_y});
    background.setFillColor(kVIEW_BOX_BACKGROUND_COLOR);
    background.setOutlineColor(kVIEW_BOX_OUTLINE_COLOR);
    background.setOutlineThickness(kVIEW_BOX_OUTLINE_THICKNESS);
    window.draw(background);
    
    // Draw title
    sf::Text title(m_font);
    title.setString("Memory");
    title.setCharacterSize(kTITLE_FONT_SIZE);
    title.setFillColor(kTITLE_TEXT_COLOR);
    title.setPosition({m_x + kPADDING, m_y + kPADDING});
    window.draw(title);
    
    // Draw column headers
    sf::Text header(m_font);
    header.setString("Address       Value        Instruction");
    header.setCharacterSize(kHEADER_FONT_SIZE);
    header.setFillColor(kHEADER_TEXT_COLOR);
    header.setPosition({m_x + kPADDING, m_y + 40.F});
    window.draw(header);
}

void MemoryView::draw_memory_word(sf::RenderWindow& window, address_t addr, float row_y, bool is_pc) {
    // Read the word from memory
    word_t value = m_memory.read_word(addr);
    
    // If this is the PC location, draw a highlight background
    if (is_pc) {
        sf::RectangleShape highlight({330.F, kROW_HEIGHT});
        highlight.setPosition({m_x + kPADDING, row_y});
        highlight.setFillColor(kPC_HIGHLIGHT_COLOR);
        highlight.setOutlineColor(kPC_HIGHLIGHT_OUTLINE);
        highlight.setOutlineThickness(1.F);
        window.draw(highlight);
    }
    
    // Format address as hex (e.g., "0x00000000")
    std::ostringstream addr_stream;
    addr_stream << "0x" << std::hex << std::setw(8) << std::setfill('0') << addr;
    
    // Format value as hex
    std::ostringstream value_stream;
    value_stream << "0x" << std::hex << std::setw(8) << std::setfill('0') << value;
    
    // Decode instruction (basic - just show opcode for now)
    std::ostringstream instr_stream;
    uint8_t opcode = (value >> 26) & 0x3F;
    instr_stream << "op=" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(opcode);
    
    // Draw address
    sf::Text addr_text(m_font);
    addr_text.setString(addr_stream.str());
    addr_text.setCharacterSize(kNORMAL_FONT_SIZE);
    addr_text.setFillColor(kNORMAL_TEXT_COLOR);
    addr_text.setPosition({m_x + kPADDING, row_y});
    window.draw(addr_text);
    
    // Draw value
    sf::Text value_text(m_font);
    value_text.setString(value_stream.str());
    value_text.setCharacterSize(kNORMAL_FONT_SIZE);
    value_text.setFillColor(kVALUE_TEXT_COLOR);
    value_text.setPosition({m_x + 120.F, row_y});
    window.draw(value_text);
    
    // Draw instruction hint
    sf::Text instr_text(m_font);
    instr_text.setString(instr_stream.str());
    instr_text.setCharacterSize(kNORMAL_FONT_SIZE);
    instr_text.setFillColor(kHEADER_TEXT_COLOR);
    instr_text.setPosition({m_x + 240.F, row_y});
    window.draw(instr_text);
}

} // namespace ez_arch