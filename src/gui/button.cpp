#include "gui/button.hpp"

#include "gui/style.hpp"

#include <utility>

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

void Button::setPosition(float x, float y) {
  m_shape.setPosition({x, y});

  // Center text in button
  sf::FloatRect textBounds = m_text.getLocalBounds();
  m_text.setOrigin({textBounds.position.x + (textBounds.size.x / 2.0F),
                    textBounds.position.y + (textBounds.size.y / 2.0F)});
  m_text.setPosition({x + (kBUTTON_WIDTH / 2.0F), y + (kBUTTON_HEIGHT / 2.0F)});
}

void Button::setSize(float width, float height) {
  m_shape.setSize({width, height});

  // Re-center text
  sf::Vector2f pos = m_shape.getPosition();
  sf::FloatRect textBounds = m_text.getLocalBounds();
  m_text.setOrigin({textBounds.position.x + (textBounds.size.x / 2.0F),
                    textBounds.position.y + (textBounds.size.y / 2.0F)});
  m_text.setPosition({pos.x + (width / 2.0F), pos.y + (height / 2.0F)});
}

void Button::setCallback(Callback callback) {
  m_callback = std::move(callback);
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
    if (m_callback) { m_callback(); }
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
    m_shape.setFillColor(kBUTTON_PRESSED_COLOR);
  } else if (m_isHovered) {
    m_shape.setFillColor(kBUTTON_HOVER_COLOR);
  } else {
    m_shape.setFillColor(kBUTTON_NORMAL_COLOR);
  }
}

} // namespace ez_arch
