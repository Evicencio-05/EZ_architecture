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
    sf::RectangleShape shape_;
    sf::Font& font_;           // Must be declared BEFORE text_
    sf::Text text_;            // Since text_ uses font_ in constructor
    Callback callback_;
    
    bool isHovered_;
    bool isPressed_;
    
    bool contains(float x, float y) const;
    void updateColor();
};

} // namespace ez_arch
