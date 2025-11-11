#include "gui/memory_view.hpp"
#include "gui/style.hpp"
#include <iomanip>
#include <sstream>

namespace ez_arch {

MemoryView::MemoryView(const Memory& memory, const RegisterFile& registers, sf::Font& font)
    : memory_(memory), registers_(registers), font_(font), 
      x_(0.f), y_(0.f), startAddr_(0), numWords_(16) {
    // Default: show first 16 words of memory
}

void MemoryView::setPosition(float x, float y) {
    x_ = x;
    y_ = y;
}

void MemoryView::setDisplayRange(address_t startAddr, size_t numWords) {
    startAddr_ = startAddr;
    numWords_ = numWords;
}

void MemoryView::update() {
    // Could auto-scroll to follow PC here in the future
}

void MemoryView::draw(sf::RenderWindow& window) {
    drawHeader(window);
    
    // Get current PC for highlighting
    word_t currentPC = registers_.get_pc();
    
    // Draw each memory word
    const float HEADER_HEIGHT = 60.f;
    
    for (size_t i = 0; i < numWords_; i++) {
        address_t addr = startAddr_ + (i * 4);  // 4 bytes per word
        float rowY = y_ + HEADER_HEIGHT + (i * ROW_HEIGHT);
        
        // Check if this address is the current PC
        bool isPC = (addr == currentPC);
        
        drawMemoryWord(window, addr, rowY, isPC);
    }
}

void MemoryView::drawHeader(sf::RenderWindow& window) {
    // Calculate background size based on number of words to display
    float height = 60.f + (numWords_ * ROW_HEIGHT) + PADDING;
    
    // Draw background box
    sf::RectangleShape background({350.f, height});
    background.setPosition({x_, y_});
    background.setFillColor(VIEW_BOX_BACKGROUND_COLOR);
    background.setOutlineColor(VIEW_BOX_OUTLINE_COLOR);
    background.setOutlineThickness(VIEW_BOX_OUTLINE_THICKNESS);
    window.draw(background);
    
    // Draw title
    sf::Text title(font_);
    title.setString("Memory");
    title.setCharacterSize(TITLE_FONT_SIZE);
    title.setFillColor(TITLE_TEXT_COLOR);
    title.setPosition({x_ + PADDING, y_ + PADDING});
    window.draw(title);
    
    // Draw column headers
    sf::Text header(font_);
    header.setString("Address       Value        Instruction");
    header.setCharacterSize(HEADER_FONT_SIZE);
    header.setFillColor(HEADER_TEXT_COLOR);
    header.setPosition({x_ + PADDING, y_ + 40.f});
    window.draw(header);
}

void MemoryView::drawMemoryWord(sf::RenderWindow& window, address_t addr, float rowY, bool isPC) {
    // Read the word from memory
    word_t value = memory_.read_word(addr);
    
    // If this is the PC location, draw a highlight background
    if (isPC) {
        sf::RectangleShape highlight({330.f, ROW_HEIGHT});
        highlight.setPosition({x_ + PADDING, rowY});
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
    sf::Text addrText(font_);
    addrText.setString(addrStream.str());
    addrText.setCharacterSize(NORMAL_FONT_SIZE);
    addrText.setFillColor(NORMAL_TEXT_COLOR);
    addrText.setPosition({x_ + PADDING, rowY});
    window.draw(addrText);
    
    // Draw value
    sf::Text valueText(font_);
    valueText.setString(valueStream.str());
    valueText.setCharacterSize(NORMAL_FONT_SIZE);
    valueText.setFillColor(VALUE_TEXT_COLOR);
    valueText.setPosition({x_ + 120.f, rowY});
    window.draw(valueText);
    
    // Draw instruction hint
    sf::Text instrText(font_);
    instrText.setString(instrStream.str());
    instrText.setCharacterSize(NORMAL_FONT_SIZE);
    instrText.setFillColor(HEADER_TEXT_COLOR);
    instrText.setPosition({x_ + 240.f, rowY});
    window.draw(instrText);
}

} // namespace ez_arch