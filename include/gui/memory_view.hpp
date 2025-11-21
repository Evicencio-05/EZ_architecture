#pragma once

#include "core/memory.hpp"
#include "core/register_file.hpp"

#include <SFML/Graphics.hpp>

namespace ez_arch {

class MemoryView {
public:
  MemoryView(const Memory& memory,
             const RegisterFile& registers,
             sf::Font& font);

  // Set position of the view on screen
  void setPosition(float x, float y);

  // Update state from memory
  void update();

  // Draw the memory view
  void draw(sf::RenderWindow& window);

  // Set which memory range to display (start address and number of words)
  void setDisplayRange(address_t startAddr, size_t numWords);

private:
  const Memory& m_memory;
  const RegisterFile& m_registers; // To get PC for highlighting
  sf::Font& m_font;
  float m_x {0.F};
  float m_y {0.F};

  // Display settings
  address_t m_startAddr {0}; // Starting address to display
  size_t m_numWords {16};    // Number of words to display

  // Drawing helpers
  void drawHeader(sf::RenderWindow& window);
  void drawMemoryWord(sf::RenderWindow& window,
                      address_t addr,
                      float rowY,
                      bool isPc);
};

} // namespace ez_arch
