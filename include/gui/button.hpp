#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <functional>

namespace ez_arch {

class Button {
public:
    using Callback = std::function<void()>;
    
    Button(const std::string& text, sf::Font& font);
    
    void setPosition(float x, float y);
    void setSize(float width, float height);
    void setCallback(Callback callback);
    
    // Event handling
    void handleMouseMove(float mouseX, float mouseY);
    void handleMousePress(float mouseX, float mouseY);
    void handleMouseRelease(float mouseX, float mouseY);
    
    void draw(sf::RenderWindow& window);
    
private:
    sf::RectangleShape m_shape;
    sf::Font& m_font;           // Must be declared BEFORE m_text
    sf::Text m_text;            // Since m_text uses m_font in constructor
    Callback m_callback;
    
    bool m_isHovered;
    bool m_isPressed;
    
    bool contains(float x, float y) const;
    void updateColor();
};

} // namespace ez_arch
