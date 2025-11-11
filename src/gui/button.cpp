#include "gui/button.hpp"
#include "gui/style.hpp"

namespace ez_arch {

Button::Button(const std::string& text, sf::Font& font)
    : font_(font), text_(font_), isHovered_(false), isPressed_(false) {
    
    // Setup shape
    shape_.setSize({BUTTON_WIDTH, BUTTON_HEIGHT});
    shape_.setFillColor(BUTTON_NORMAL_COLOR);
    
    // Setup text
    text_.setString(text);
    text_.setCharacterSize(14);
    text_.setFillColor(BUTTON_TEXT_COLOR);
}

void Button::setPosition(float x, float y) {
    shape_.setPosition({x, y});
    
    // Center text in button
    sf::FloatRect textBounds = text_.getLocalBounds();
    text_.setOrigin({
        textBounds.position.x + textBounds.size.x / 2.0f,
        textBounds.position.y + textBounds.size.y / 2.0f
    });
    text_.setPosition({
        x + BUTTON_WIDTH / 2.0f,
        y + BUTTON_HEIGHT / 2.0f
    });
}

void Button::setSize(float width, float height) {
    shape_.setSize({width, height});
    
    // Re-center text
    sf::Vector2f pos = shape_.getPosition();
    sf::FloatRect textBounds = text_.getLocalBounds();
    text_.setOrigin({
        textBounds.position.x + textBounds.size.x / 2.0f,
        textBounds.position.y + textBounds.size.y / 2.0f
    });
    text_.setPosition({
        pos.x + width / 2.0f,
        pos.y + height / 2.0f
    });
}

void Button::setCallback(Callback callback) {
    callback_ = callback;
}

void Button::handleMouseMove(float mouseX, float mouseY) {
    isHovered_ = contains(mouseX, mouseY);
    updateColor();
}

void Button::handleMousePress(float mouseX, float mouseY) {
    if (contains(mouseX, mouseY)) {
        isPressed_ = true;
        updateColor();
    }
}

void Button::handleMouseRelease(float mouseX, float mouseY) {
    if (isPressed_ && contains(mouseX, mouseY)) {
        // Button was clicked!
        if (callback_) {
            callback_();
        }
    }
    isPressed_ = false;
    updateColor();
}

void Button::draw(sf::RenderWindow& window) {
    window.draw(shape_);
    window.draw(text_);
}

bool Button::contains(float x, float y) const {
    sf::FloatRect bounds = shape_.getGlobalBounds();
    return bounds.contains({x, y});
}

void Button::updateColor() {
    if (isPressed_) {
        shape_.setFillColor(BUTTON_PRESSED_COLOR);
    } else if (isHovered_) {
        shape_.setFillColor(BUTTON_HOVER_COLOR);
    } else {
        shape_.setFillColor(BUTTON_NORMAL_COLOR);
    }
}

} // namespace ez_arch
