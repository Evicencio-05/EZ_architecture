 #pragma once

#include <SFML/Graphics.hpp>
#include "core/register_file.hpp"

namespace ez_arch {

class RegisterView {
public:
    RegisterView(const RegisterFile& registers, sf::Font& font);
    
    // Set position of the view on screen
    void setPosition(float x, float y);
    
    // Update state from register file (for future: detect changes, etc.)
    void update();
    
    // Draw the register view
    void draw(sf::RenderWindow& window);
    
private:
    const RegisterFile& registers_;
    sf::Font& font_;
    float x_;
    float y_;
    
    // Drawing helpers
    void drawRegister(sf::RenderWindow& window, int regNum, float rowY);
    void drawHeader(sf::RenderWindow& window);
};

} // namespace ez_arch
