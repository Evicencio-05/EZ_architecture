#pragma once

#include "core/memory.hpp"
#include "core/register_file.hpp"

#include <SFML/Graphics.hpp>
#include <string>

namespace ez_arch {

class InstructionView {
public:
  InstructionView(const Memory& memory,
                  const RegisterFile& registers,
                  sf::Font& font);

  // Set position of the view on screen
  void setPosition(float x, float y);

  // Update state
  void update();

  // Draw the instruction view
  void draw(sf::RenderWindow& window);

  // Set which instruction range to display (start address and count)
  void setDisplayRange(address_t startAddr, size_t numInstructions);

private:
  const Memory& m_memory;
  const RegisterFile& m_registers; // To get PC for highlighting
  sf::Font& m_font;
  float m_x {0.F};
  float m_y {0.F};

  // Display settings
  address_t m_startAddr {0};
  size_t m_numInstructions {16};

  // Drawing helpers
  void drawHeader(sf::RenderWindow& window);
  void drawInstruction(sf::RenderWindow& window,
                       address_t addr,
                       float rowY,
                       bool isPc);

  // Instruction decoding helper
  [[nodiscard]] static std::string decodeInstruction(word_t instruction);
};

} // namespace ez_arch
