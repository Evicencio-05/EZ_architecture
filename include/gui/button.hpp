#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <functional>

namespace ez_arch {

class Button {
public:
    using Callback = std::function<void()>;
    
    Button(const std::string& text, sf::Font& font);
    
    void set_position(float x, float y);
    void set_size(float width, float height);
    void set_callback(Callback callback);
    
    // Event handling
    void handle_mouse_move(float mouse_x, float mouse_y);
    void handle_mouse_press(float mouse_x, float mouse_y);
    void handle_mouse_release(float mouse_x, float mouse_y);
    
    void draw(sf::RenderWindow& window);
    
private:
    sf::RectangleShape m_shape;
    sf::Font& m_font;           // Must be declared BEFORE m_text
    sf::Text m_text;            // Since m_text uses m_font in constructor
    Callback m_callback;
    
    bool m_isHovered{false};
    bool m_isPressed{false};
    
    bool contains(float x, float y) const;
    void update_color();
};

} // namespace ez_arch
