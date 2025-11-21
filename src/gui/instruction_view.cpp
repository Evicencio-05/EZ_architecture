#include "gui/instruction_view.hpp"

#include "core/decoder.hpp"
#include "gui/style.hpp"

#include <iomanip>
#include <sstream>

namespace ez_arch {

InstructionView::InstructionView(const Memory& memory,
                                 const RegisterFile& registers,
                                 sf::Font& font)
    : m_memory(memory), m_registers(registers), m_font(font) {
  // Default: show first 16 instructions
}

void InstructionView::setPosition(float x, float y) {
  m_x = x;
  m_y = y;
}

void InstructionView::setDisplayRange(address_t startAddr,
                                      size_t numInstructions) {
  m_startAddr = startAddr;
  m_numInstructions = numInstructions;
}

void InstructionView::update() {
  // Could auto-scroll to follow PC here in the future
}

void InstructionView::draw(sf::RenderWindow& window) {
  drawHeader(window);

  // Get current PC for highlighting
  word_t currentPc = m_registers.getPc();

  // Draw each instruction
  const float kHEADER_HEIGHT = 60.F;

  for (size_t i = 0; i < m_numInstructions; i++) {
    address_t addr = m_startAddr + (i * 4); // 4 bytes per instruction
    float rowY = m_y + kHEADER_HEIGHT + (i * kROW_HEIGHT);

    // Check if this address is the current PC
    bool isPc = (addr == currentPc);

    drawInstruction(window, addr, rowY, isPc);
  }
}

void InstructionView::drawHeader(sf::RenderWindow& window) {
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

void InstructionView::drawInstruction(sf::RenderWindow& window,
                                      address_t addr,
                                      float rowY,
                                      bool isPc) {
  // Read the instruction word from memory
  word_t instructionWord = m_memory.readWord(addr);

  // If this is the PC location, draw a highlight background
  if (isPc) {
    sf::RectangleShape highlight({480.F, kROW_HEIGHT});
    highlight.setPosition({m_x + kPADDING, rowY});
    highlight.setFillColor(kPC_HIGHLIGHT_COLOR);
    highlight.setOutlineColor(kPC_HIGHLIGHT_OUTLINE);
    highlight.setOutlineThickness(1.F);
    window.draw(highlight);
  }

  // Format address as hex
  std::ostringstream addrStream;
  addrStream << "0x" << std::hex << std::setw(8) << std::setfill('0') << addr;

  // Format instruction word as hex
  std::ostringstream hexStream;
  hexStream << "0x" << std::hex << std::setw(8) << std::setfill('0')
            << instructionWord;

  // Decode instruction to assembly
  std::string assembly = decodeInstruction(instructionWord);

  // Draw address
  sf::Text addrText(m_font);
  addrText.setString(addrStream.str());
  addrText.setCharacterSize(kNORMAL_FONT_SIZE);
  addrText.setFillColor(kNORMAL_TEXT_COLOR);
  addrText.setPosition({m_x + kPADDING, rowY});
  window.draw(addrText);

  // Draw hex encoding
  sf::Text hexText(m_font);
  hexText.setString(hexStream.str());
  hexText.setCharacterSize(kNORMAL_FONT_SIZE);
  hexText.setFillColor(kHEADER_TEXT_COLOR);
  hexText.setPosition({m_x + 120.F, rowY});
  window.draw(hexText);

  // Draw assembly instruction
  sf::Text asmText(m_font);
  asmText.setString(assembly);
  asmText.setCharacterSize(kNORMAL_FONT_SIZE);
  asmText.setFillColor(isPc ? sf::Color::Black
                            : kVALUE_TEXT_COLOR); // Black if PC for contrast
  asmText.setPosition({m_x + 240.F, rowY});
  window.draw(asmText);
}

std::string InstructionView::decodeInstruction(word_t instruction) {
  // Handle NOP (all zeros) specially
  if (instruction == 0) { return "nop"; }

  // Use the Decoder to decode the instruction
  try {
    return Decoder::decode(instruction);
  } catch (...) {
    // If decoding fails, return a generic format
    return "???";
  }
}

} // namespace ez_arch
