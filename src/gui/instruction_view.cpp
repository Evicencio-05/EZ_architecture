#include "gui/instruction_view.hpp"
#include "gui/style.hpp"
#include "core/decoder.hpp"
#include <iomanip>
#include <sstream>

namespace ez_arch {

InstructionView::InstructionView(const Memory& memory, const RegisterFile& registers, sf::Font& font)
    : m_memory(memory), m_registers(registers), m_font(font),
      m_x(0.f), m_y(0.f), m_startAddr(0), m_numInstructions(16) {
    // Default: show first 16 instructions
}

void InstructionView::setPosition(float x, float y) {
    m_x = x;
    m_y = y;
}

void InstructionView::setDisplayRange(address_t startAddr, size_t numInstructions) {
    m_startAddr = startAddr;
    m_numInstructions = numInstructions;
}

void InstructionView::update() {
    // Could auto-scroll to follow PC here in the future
}

void InstructionView::draw(sf::RenderWindow& window) {
    drawHeader(window);
    
    // Get current PC for highlighting
    word_t currentPC = m_registers.get_pc();
    
    // Draw each instruction
    const float HEADER_HEIGHT = 60.f;
    
    for (size_t i = 0; i < m_numInstructions; i++) {
        address_t addr = m_startAddr + (i * 4);  // 4 bytes per instruction
        float rowY = m_y + HEADER_HEIGHT + (i * ROW_HEIGHT);
        
        // Check if this address is the current PC
        bool isPC = (addr == currentPC);
        
        drawInstruction(window, addr, rowY, isPC);
    }
}

void InstructionView::drawHeader(sf::RenderWindow& window) {
    // Calculate background size based on number of instructions
    float height = 60.f + (m_numInstructions * ROW_HEIGHT) + PADDING;
    
    // Draw background box
    sf::RectangleShape background({500.f, height});
    background.setPosition({m_x, m_y});
    background.setFillColor(VIEW_BOX_BACKGROUND_COLOR);
    background.setOutlineColor(VIEW_BOX_OUTLINE_COLOR);
    background.setOutlineThickness(VIEW_BOX_OUTLINE_THICKNESS);
    window.draw(background);
    
    // Draw title
    sf::Text title(m_font);
    title.setString("Instructions");
    title.setCharacterSize(TITLE_FONT_SIZE);
    title.setFillColor(TITLE_TEXT_COLOR);
    title.setPosition({m_x + PADDING, m_y + PADDING});
    window.draw(title);
    
    // Draw column headers
    sf::Text header(m_font);
    header.setString("Address       Hex          Assembly");
    header.setCharacterSize(HEADER_FONT_SIZE);
    header.setFillColor(HEADER_TEXT_COLOR);
    header.setPosition({m_x + PADDING, m_y + 40.f});
    window.draw(header);
}

void InstructionView::drawInstruction(sf::RenderWindow& window, address_t addr, float rowY, bool isPC) {
    // Read the instruction word from memory
    word_t instructionWord = m_memory.read_word(addr);
    
    // If this is the PC location, draw a highlight background
    if (isPC) {
        sf::RectangleShape highlight({480.f, ROW_HEIGHT});
        highlight.setPosition({m_x + PADDING, rowY});
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
    sf::Text addrText(m_font);
    addrText.setString(addrStream.str());
    addrText.setCharacterSize(NORMAL_FONT_SIZE);
    addrText.setFillColor(NORMAL_TEXT_COLOR);
    addrText.setPosition({m_x + PADDING, rowY});
    window.draw(addrText);
    
    // Draw hex encoding
    sf::Text hexText(m_font);
    hexText.setString(hexStream.str());
    hexText.setCharacterSize(NORMAL_FONT_SIZE);
    hexText.setFillColor(HEADER_TEXT_COLOR);
    hexText.setPosition({m_x + 120.f, rowY});
    window.draw(hexText);
    
    // Draw assembly instruction
    sf::Text asmText(m_font);
    asmText.setString(assembly);
    asmText.setCharacterSize(NORMAL_FONT_SIZE);
    asmText.setFillColor(isPC ? sf::Color::Black : VALUE_TEXT_COLOR);  // Black if PC for contrast
    asmText.setPosition({m_x + 240.f, rowY});
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