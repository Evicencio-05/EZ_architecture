#include "gui/memory_view.hpp"
#include "gui/style.hpp"
#include <iomanip>
#include <sstream>

namespace ez_arch {

MemoryView::MemoryView(const Memory& memory, const RegisterFile& registers, sf::Font& font)
    : m_memory(memory), m_registers(registers), m_font(font), 
      m_x(0.f), m_y(0.f), m_startAddr(0), m_numWords(16) {
    // Default: show first 16 words of memory
}

void MemoryView::setPosition(float x, float y) {
    m_x = x;
    m_y = y;
}

void MemoryView::setDisplayRange(address_t startAddr, size_t numWords) {
    m_startAddr = startAddr;
    m_numWords = numWords;
}

void MemoryView::update() {
    // Could auto-scroll to follow PC here in the future
}

void MemoryView::draw(sf::RenderWindow& window) {
    drawHeader(window);
    
    // Get current PC for highlighting
    word_t currentPC = m_registers.get_pc();
    
    // Draw each memory word
    const float HEADER_HEIGHT = 60.f;
    
    for (size_t i = 0; i < m_numWords; i++) {
        address_t addr = m_startAddr + (i * 4);  // 4 bytes per word
        float rowY = m_y + HEADER_HEIGHT + (i * ROW_HEIGHT);
        
        // Check if this address is the current PC
        bool isPC = (addr == currentPC);
        
        drawMemoryWord(window, addr, rowY, isPC);
    }
}

void MemoryView::drawHeader(sf::RenderWindow& window) {
    // Calculate background size based on number of words to display
    float height = 60.f + (m_numWords * ROW_HEIGHT) + PADDING;
    
    // Draw background box
    sf::RectangleShape background({350.f, height});
    background.setPosition({m_x, m_y});
    background.setFillColor(VIEW_BOX_BACKGROUND_COLOR);
    background.setOutlineColor(VIEW_BOX_OUTLINE_COLOR);
    background.setOutlineThickness(VIEW_BOX_OUTLINE_THICKNESS);
    window.draw(background);
    
    // Draw title
    sf::Text title(m_font);
    title.setString("Memory");
    title.setCharacterSize(TITLE_FONT_SIZE);
    title.setFillColor(TITLE_TEXT_COLOR);
    title.setPosition({m_x + PADDING, m_y + PADDING});
    window.draw(title);
    
    // Draw column headers
    sf::Text header(m_font);
    header.setString("Address       Value        Instruction");
    header.setCharacterSize(HEADER_FONT_SIZE);
    header.setFillColor(HEADER_TEXT_COLOR);
    header.setPosition({m_x + PADDING, m_y + 40.f});
    window.draw(header);
}

void MemoryView::drawMemoryWord(sf::RenderWindow& window, address_t addr, float rowY, bool isPC) {
    // Read the word from memory
    word_t value = m_memory.read_word(addr);
    
    // If this is the PC location, draw a highlight background
    if (isPC) {
        sf::RectangleShape highlight({330.f, ROW_HEIGHT});
        highlight.setPosition({m_x + PADDING, rowY});
        highlight.setFillColor(PC_HIGHLIGHT_COLOR);
        highlight.setOutlineColor(PC_HIGHLIGHT_OUTLINE);
        highlight.setOutlineThickness(1.f);
        window.draw(highlight);
    }
    
    // Format address as hex (e.g., "0x00000000")
    std::ostringstream addrStream;
    addrStream << "0x" << std::hex << std::setw(8) << std::setfill('0') << addr;
    
    // Format value as hex
    std::ostringstream valueStream;
    valueStream << "0x" << std::hex << std::setw(8) << std::setfill('0') << value;
    
    // Decode instruction (basic - just show opcode for now)
    std::ostringstream instrStream;
    uint8_t opcode = (value >> 26) & 0x3F;
    instrStream << "op=" << std::hex << std::setw(2) << std::setfill('0') << (int)opcode;
    
    // Draw address
    sf::Text addrText(m_font);
    addrText.setString(addrStream.str());
    addrText.setCharacterSize(NORMAL_FONT_SIZE);
    addrText.setFillColor(NORMAL_TEXT_COLOR);
    addrText.setPosition({m_x + PADDING, rowY});
    window.draw(addrText);
    
    // Draw value
    sf::Text valueText(m_font);
    valueText.setString(valueStream.str());
    valueText.setCharacterSize(NORMAL_FONT_SIZE);
    valueText.setFillColor(VALUE_TEXT_COLOR);
    valueText.setPosition({m_x + 120.f, rowY});
    window.draw(valueText);
    
    // Draw instruction hint
    sf::Text instrText(m_font);
    instrText.setString(instrStream.str());
    instrText.setCharacterSize(NORMAL_FONT_SIZE);
    instrText.setFillColor(HEADER_TEXT_COLOR);
    instrText.setPosition({m_x + 240.f, rowY});
    window.draw(instrText);
}

} // namespace ez_arch