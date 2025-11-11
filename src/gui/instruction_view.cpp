#include "gui/instruction_view.hpp"
#include "gui/style.hpp"
#include "core/decoder.hpp"
#include <iomanip>
#include <sstream>

namespace ez_arch {

InstructionView::InstructionView(const Memory& memory, const RegisterFile& registers, sf::Font& font)
    : memory_(memory), registers_(registers), font_(font),
      x_(0.f), y_(0.f), startAddr_(0), numInstructions_(16) {
    // Default: show first 16 instructions
}

void InstructionView::setPosition(float x, float y) {
    x_ = x;
    y_ = y;
}

void InstructionView::setDisplayRange(address_t startAddr, size_t numInstructions) {
    startAddr_ = startAddr;
    numInstructions_ = numInstructions;
}

void InstructionView::update() {
    // Could auto-scroll to follow PC here in the future
}

void InstructionView::draw(sf::RenderWindow& window) {
    drawHeader(window);
    
    // Get current PC for highlighting
    word_t currentPC = registers_.get_pc();
    
    // Draw each instruction
    const float HEADER_HEIGHT = 60.f;
    
    for (size_t i = 0; i < numInstructions_; i++) {
        address_t addr = startAddr_ + (i * 4);  // 4 bytes per instruction
        float rowY = y_ + HEADER_HEIGHT + (i * ROW_HEIGHT);
        
        // Check if this address is the current PC
        bool isPC = (addr == currentPC);
        
        drawInstruction(window, addr, rowY, isPC);
    }
}

void InstructionView::drawHeader(sf::RenderWindow& window) {
    // Calculate background size based on number of instructions
    float height = 60.f + (numInstructions_ * ROW_HEIGHT) + PADDING;
    
    // Draw background box
    sf::RectangleShape background({500.f, height});
    background.setPosition({x_, y_});
    background.setFillColor(VIEW_BOX_BACKGROUND_COLOR);
    background.setOutlineColor(VIEW_BOX_OUTLINE_COLOR);
    background.setOutlineThickness(VIEW_BOX_OUTLINE_THICKNESS);
    window.draw(background);
    
    // Draw title
    sf::Text title(font_);
    title.setString("Instructions");
    title.setCharacterSize(TITLE_FONT_SIZE);
    title.setFillColor(TITLE_TEXT_COLOR);
    title.setPosition({x_ + PADDING, y_ + PADDING});
    window.draw(title);
    
    // Draw column headers
    sf::Text header(font_);
    header.setString("Address       Hex          Assembly");
    header.setCharacterSize(HEADER_FONT_SIZE);
    header.setFillColor(HEADER_TEXT_COLOR);
    header.setPosition({x_ + PADDING, y_ + 40.f});
    window.draw(header);
}

void InstructionView::drawInstruction(sf::RenderWindow& window, address_t addr, float rowY, bool isPC) {
    // Read the instruction word from memory
    word_t instructionWord = memory_.read_word(addr);
    
    // If this is the PC location, draw a highlight background
    if (isPC) {
        sf::RectangleShape highlight({480.f, ROW_HEIGHT});
        highlight.setPosition({x_ + PADDING, rowY});
        highlight.setFillColor(PC_HIGHLIGHT_COLOR);
        highlight.setOutlineColor(PC_HIGHLIGHT_OUTLINE);
        highlight.setOutlineThickness(1.f);
        window.draw(highlight);
    }
    
    // Format address as hex
    std::ostringstream addrStream;
    addrStream << "0x" << std::hex << std::setw(8) << std::setfill('0') << addr;
    
    // Format instruction word as hex
    std::ostringstream hexStream;
    hexStream << "0x" << std::hex << std::setw(8) << std::setfill('0') << instructionWord;
    
    // Decode instruction to assembly
    std::string assembly = decodeInstruction(instructionWord);
    
    // Draw address
    sf::Text addrText(font_);
    addrText.setString(addrStream.str());
    addrText.setCharacterSize(NORMAL_FONT_SIZE);
    addrText.setFillColor(NORMAL_TEXT_COLOR);
    addrText.setPosition({x_ + PADDING, rowY});
    window.draw(addrText);
    
    // Draw hex encoding
    sf::Text hexText(font_);
    hexText.setString(hexStream.str());
    hexText.setCharacterSize(NORMAL_FONT_SIZE);
    hexText.setFillColor(HEADER_TEXT_COLOR);
    hexText.setPosition({x_ + 120.f, rowY});
    window.draw(hexText);
    
    // Draw assembly instruction
    sf::Text asmText(font_);
    asmText.setString(assembly);
    asmText.setCharacterSize(NORMAL_FONT_SIZE);
    asmText.setFillColor(isPC ? sf::Color::Black : VALUE_TEXT_COLOR);  // Black if PC for contrast
    asmText.setPosition({x_ + 240.f, rowY});
    window.draw(asmText);
}

std::string InstructionView::decodeInstruction(word_t instruction) const {
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