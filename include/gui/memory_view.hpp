#pragma once

#include <SFML/Graphics.hpp>
#include "core/memory.hpp"
#include "core/register_file.hpp"

namespace ez_arch {

class MemoryView {
public:
    MemoryView(const Memory& memory, const RegisterFile& registers, sf::Font& font);
    
    // Set position of the view on screen
    void setPosition(float x, float y);
    
    // Update state from memory
    void update();
    
    // Draw the memory view
    void draw(sf::RenderWindow& window);
    
    // Set which memory range to display (start address and number of words)
    void setDisplayRange(address_t startAddr, size_t numWords);
    
private:
    const Memory& memory_;
    const RegisterFile& registers_;  // To get PC for highlighting
    sf::Font& font_;
    float x_;
    float y_;
    
    // Display settings
    address_t startAddr_;    // Starting address to display
    size_t numWords_;        // Number of words to display
    
    // Drawing helpers
    void drawHeader(sf::RenderWindow& window);
    void drawMemoryWord(sf::RenderWindow& window, address_t addr, float rowY, bool isPC);
};

} // namespace ez_arch
