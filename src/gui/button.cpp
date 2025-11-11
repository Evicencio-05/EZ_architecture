#include "gui/button.hpp"
#include "gui/style.hpp"

namespace ez_arch {

Button::Button(const std::string& text, sf::Font& font)
    : m_font(font), m_text(m_font), m_isHovered(false), m_isPressed(false) {
    
    // Setup shape
    m_shape.setSize({BUTTON_WIDTH, BUTTON_HEIGHT});
    m_shape.setFillColor(BUTTON_NORMAL_COLOR);
    
    // Setup text
    m_text.setString(text);
    m_text.setCharacterSize(14);
    m_text.setFillColor(BUTTON_TEXT_COLOR);
}

void Button::setPosition(float x, float y) {
    m_shape.setPosition({x, y});
    
    // Center text in button
    sf::FloatRect textBounds = m_text.getLocalBounds();
    m_text.setOrigin({
        textBounds.position.x + textBounds.size.x / 2.0f,
        textBounds.position.y + textBounds.size.y / 2.0f
    });
    m_text.setPosition({
        x + BUTTON_WIDTH / 2.0f,
        y + BUTTON_HEIGHT / 2.0f
    });
}

void Button::setSize(float width, float height) {
    m_shape.setSize({width, height});
    
    // Re-center text
    sf::Vector2f pos = m_shape.getPosition();
    sf::FloatRect textBounds = m_text.getLocalBounds();
    m_text.setOrigin({
        textBounds.position.x + textBounds.size.x / 2.0f,
        textBounds.position.y + textBounds.size.y / 2.0f
    });
    m_text.setPosition({
        pos.x + width / 2.0f,
        pos.y + height / 2.0f
    });
}

void Button::setCallback(Callback callback) {
    m_callback = callback;
}

void Button::handleMouseMove(float mouseX, float mouseY) {
    m_isHovered = contains(mouseX, mouseY);
    updateColor();
}

void Button::handleMousePress(float mouseX, float mouseY) {
    if (contains(mouseX, mouseY)) {
        m_isPressed = true;
        updateColor();
    }
}

void Button::handleMouseRelease(float mouseX, float mouseY) {
    if (m_isPressed && contains(mouseX, mouseY)) {
        // Button was clicked!
        if (m_callback) {
            m_callback();
        }
    }
    m_isPressed = false;
    updateColor();
}

void Button::draw(sf::RenderWindow& window) {
    window.draw(m_shape);
    window.draw(m_text);
}

bool Button::contains(float x, float y) const {
    sf::FloatRect bounds = m_shape.getGlobalBounds();
    return bounds.contains({x, y});
}

void Button::updateColor() {
    if (m_isPressed) {
        m_shape.setFillColor(BUTTON_PRESSED_COLOR);
    } else if (m_isHovered) {
        m_shape.setFillColor(BUTTON_HOVER_COLOR);
    } else {
        m_shape.setFillColor(BUTTON_NORMAL_COLOR);
    }
}

} // namespace ez_arch
