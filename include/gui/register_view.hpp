 #pragma once

#include <SFML/Graphics.hpp>
#include "core/register_file.hpp"

namespace ez_arch {

class RegisterView {
public:
    RegisterView(const RegisterFile& registers, sf::Font& font);
    
    // Set position of the view on screen
    void set_position(float x, float y);
    
    // Update state from register file (for future: detect changes, etc.)
    void update();
    
    // Draw the register view
    void draw(sf::RenderWindow& window);
    
private:
    const RegisterFile& m_registers;
    sf::Font& m_font;
    float m_x{0.F};
    float m_y{0.F};
    
    // Drawing helpers
    void draw_register(sf::RenderWindow& window, int reg_num, float row_y);
    void draw_header(sf::RenderWindow& window);
};

} // namespace ez_arch
