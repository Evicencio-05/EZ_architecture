#include "gui/register_view.hpp"

#include "core/register_names.hpp"
#include "gui/style.hpp"

#include <iomanip>
#include <sstream>

namespace ez_arch {

RegisterView::RegisterView(const RegisterFile& registers, sf::Font& font)
    : m_registers(registers), m_font(font) {}

void RegisterView::setPosition(float x, float y) {
  m_x = x;
  m_y = y;
}

void RegisterView::update() {
  // For now, nothing to do here
  // In the future, we could detect which registers changed and highlight them
}

void RegisterView::draw(sf::RenderWindow& window) {
  // Draw the header (title + column labels)
  drawHeader(window);

  // Draw all 32 registers in a table
  const float kROW_HEIGHT = 17.F;
  const float kHEADER_HEIGHT = 60.F;

  for (int i = 0; i < 32; i++) {
    float regY = m_y + kHEADER_HEIGHT + (i * kROW_HEIGHT);
    drawRegister(window, i, regY);
  }
}

void RegisterView::drawHeader(sf::RenderWindow& window) {
  // Draw background box for the entire register view
  sf::RectangleShape background({180.F, 610.F});
  background.setPosition({m_x, m_y});
  background.setFillColor(kVIEW_BOX_BACKGROUND_COLOR);
  background.setOutlineColor(kVIEW_BOX_OUTLINE_COLOR);
  background.setOutlineThickness(2.F);
  window.draw(background);

  // Draw title
  sf::Text title(m_font);
  title.setString("Registers");
  title.setCharacterSize(20);
  title.setFillColor(kTITLE_TEXT_COLOR);
  title.setPosition({m_x + 10.F, m_y + 10.F});
  window.draw(title);

  // Draw column headers
  sf::Text header(m_font);
  header.setString("Reg    Name   Value");
  header.setCharacterSize(14);
  header.setFillColor(sf::Color::Black);
  header.setPosition({m_x + 10.F, m_y + 40.F});
  window.draw(header);
}

void RegisterView::drawRegister(sf::RenderWindow& window,
                                int regNum,
                                float rowY) {
  // Read the register value
  word_t value = m_registers.read(regNum);

  // Get register name
  std::string_view regName = kREGISTER_NAMES[regNum];

  // Format register number (e.g., "$0", "$1", etc.)
  std::ostringstream regNumStream;
  regNumStream << regNum;

  // Format value as hexadecimal (e.g., "0x00000000")
  std::ostringstream valueStream;
  valueStream << "0x" << std::hex << std::setw(8) << std::setfill('0') << value;

  // Draw register number
  sf::Text regNumText(m_font);
  regNumText.setString(regNumStream.str());
  regNumText.setCharacterSize(12);
  regNumText.setFillColor(sf::Color::Black);
  regNumText.setPosition({m_x + 10.F, rowY});
  window.draw(regNumText);

  // Draw register name
  sf::Text nameText(m_font);
  nameText.setString(std::string(regName));
  nameText.setCharacterSize(12);
  nameText.setFillColor(sf::Color::Black);
  nameText.setPosition({m_x + 50.F, rowY});
  window.draw(nameText);

  // Draw register value
  sf::Text valueText(m_font);
  valueText.setString(valueStream.str());
  valueText.setCharacterSize(12);
  valueText.setFillColor(sf::Color::Black);
  valueText.setPosition({m_x + 100.F, rowY});
  window.draw(valueText);
}

} // namespace ez_arch
