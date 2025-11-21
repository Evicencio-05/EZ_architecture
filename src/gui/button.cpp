#include "gui/button.hpp"

#include <utility>
#include "gui/style.hpp"

namespace ez_arch {

Button::Button(const std::string& text, sf::Font& font)
    : m_font(font), m_text(m_font) {
    
    // Setup shape
    m_shape.setSize({kBUTTON_WIDTH, kBUTTON_HEIGHT});
    m_shape.setFillColor(kBUTTON_NORMAL_COLOR);
    
    // Setup text
    m_text.setString(text);
    m_text.setCharacterSize(14);
    m_text.setFillColor(kBUTTON_TEXT_COLOR);
}

void Button::set_position(float x, float y) {
    m_shape.setPosition({x, y});
    
    // Center text in button
    sf::FloatRect text_bounds = m_text.getLocalBounds();
    m_text.setOrigin({
        text_bounds.position.x + (text_bounds.size.x / 2.0F),
        text_bounds.position.y + (text_bounds.size.y / 2.0F)
    });
    m_text.setPosition({
        x + (kBUTTON_WIDTH / 2.0F),
        y + (kBUTTON_HEIGHT / 2.0F)
    });
}

void Button::set_size(float width, float height) {
    m_shape.setSize({width, height});
    
    // Re-center text
    sf::Vector2f pos = m_shape.getPosition();
    sf::FloatRect text_bounds = m_text.getLocalBounds();
    m_text.setOrigin({
        text_bounds.position.x + (text_bounds.size.x / 2.0F),
        text_bounds.position.y + (text_bounds.size.y / 2.0F)
    });
    m_text.setPosition({
        pos.x + (width / 2.0F),
        pos.y + (height / 2.0F)
    });
}

void Button::set_callback(Callback callback) {
    m_callback = std::move(callback);
}

void Button::handle_mouse_move(float mouse_x, float mouse_y) {
    m_isHovered = contains(mouse_x, mouse_y);
    update_color();
}

void Button::handle_mouse_press(float mouse_x, float mouse_y) {
    if (contains(mouse_x, mouse_y)) {
        m_isPressed = true;
        update_color();
    }
}

void Button::handle_mouse_release(float mouse_x, float mouse_y) {
    if (m_isPressed && contains(mouse_x, mouse_y)) {
        // Button was clicked!
        if (m_callback) {
            m_callback();
        }
    }
    m_isPressed = false;
    update_color();
}

void Button::draw(sf::RenderWindow& window) {
    window.draw(m_shape);
    window.draw(m_text);
}

bool Button::contains(float x, float y) const {
    sf::FloatRect bounds = m_shape.getGlobalBounds();
    return bounds.contains({x, y});
}

void Button::update_color() {
    if (m_isPressed) {
        m_shape.setFillColor(kBUTTON_PRESSED_COLOR);
    } else if (m_isHovered) {
        m_shape.setFillColor(kBUTTON_HOVER_COLOR);
    } else {
        m_shape.setFillColor(kBUTTON_NORMAL_COLOR);
    }
}

} // namespace ez_arch
