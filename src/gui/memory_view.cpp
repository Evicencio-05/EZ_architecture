#include "gui/memory_view.hpp"

#include "gui/style.hpp"

#include <iomanip>
#include <sstream>

namespace ez_arch {

MemoryView::MemoryView(const Memory& memory,
                       const RegisterFile& registers,
                       sf::Font& font)
    : m_memory(memory), m_registers(registers), m_font(font) {
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
  word_t currentPc = m_registers.getPc();

  // Draw each memory word
  const float kHEADER_HEIGHT = 60.F;

  for (size_t i = 0; i < m_numWords; i++) {
    address_t addr = m_startAddr + (i * 4); // 4 bytes per word
    float rowY = m_y + kHEADER_HEIGHT + (i * kROW_HEIGHT);

    // Check if this address is the current PC
    bool isPc = (addr == currentPc);

    drawMemoryWord(window, addr, rowY, isPc);
  }
}

void MemoryView::drawHeader(sf::RenderWindow& window) {
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

void MemoryView::drawMemoryWord(sf::RenderWindow& window,
                                address_t addr,
                                float rowY,
                                bool isPc) {
  // Read the word from memory
  word_t value = m_memory.readWord(addr);

  // If this is the PC location, draw a highlight background
  if (isPc) {
    sf::RectangleShape highlight({330.F, kROW_HEIGHT});
    highlight.setPosition({m_x + kPADDING, rowY});
    highlight.setFillColor(kPC_HIGHLIGHT_COLOR);
    highlight.setOutlineColor(kPC_HIGHLIGHT_OUTLINE);
    highlight.setOutlineThickness(1.F);
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
  instrStream << "op=" << std::hex << std::setw(2) << std::setfill('0')
              << static_cast<int>(opcode);

  // Draw address
  sf::Text addrText(m_font);
  addrText.setString(addrStream.str());
  addrText.setCharacterSize(kNORMAL_FONT_SIZE);
  addrText.setFillColor(kNORMAL_TEXT_COLOR);
  addrText.setPosition({m_x + kPADDING, rowY});
  window.draw(addrText);

  // Draw value
  sf::Text valueText(m_font);
  valueText.setString(valueStream.str());
  valueText.setCharacterSize(kNORMAL_FONT_SIZE);
  valueText.setFillColor(kVALUE_TEXT_COLOR);
  valueText.setPosition({m_x + 120.F, rowY});
  window.draw(valueText);

  // Draw instruction hint
  sf::Text instrText(m_font);
  instrText.setString(instrStream.str());
  instrText.setCharacterSize(kNORMAL_FONT_SIZE);
  instrText.setFillColor(kHEADER_TEXT_COLOR);
  instrText.setPosition({m_x + 240.F, rowY});
  window.draw(instrText);
}

} // namespace ez_arch
