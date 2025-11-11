#include <iostream>
#include <string>
#include "gui/datapath_view.hpp"
#include "gui/style.hpp"

namespace ez_arch {

DatapathView::DatapathView(const CPU& cpu, sf::Font& font)
    : m_cpu(cpu), m_font(font), m_x(0.f), m_y(0.f), m_width(800.f), m_height(600.f) {
    calculateLayout();
    setupWires();
}

void DatapathView::setPosition(float x, float y) {
    m_x = x;
    m_y = y;
    calculateLayout();
    setupWires();
}

void DatapathView::setSize(float width, float height) {
    m_width = width;
    m_height = height;
    calculateLayout();
    setupWires();
}

void DatapathView::drawScaffolding(sf::RenderWindow& window, sf::Vector2f mainAreaSize) {
  int width = static_cast<int>(mainAreaSize.y);
  int height = static_cast<int>(mainAreaSize.x);
  for (int i = 0; i < height; i += 50) {
    for (int j = 0; j < width; j += 50) {
      if (i == 0) {
        sf::Text text(m_font, std::to_string(j), 10);
        text.setFillColor(sf::Color::Black);
        text.setPosition(sf::Vector2f(static_cast<float>(i) + m_x, static_cast<float>(j) + m_y));
        window.draw(text);
      } else if (j == 0 ) {
        sf::Text text(m_font, std::to_string(i), 10);
        text.setFillColor(sf::Color::Black);
        text.setPosition(sf::Vector2f(static_cast<float>(i) + m_x, static_cast<float>(j) + m_y));
        window.draw(text);
      }
      sf::Vertex point{{static_cast<float>(i) + m_x, static_cast<float>(j) + m_y}, sf::Color::Black};
      window.draw(&point, 1, sf::PrimitiveType::Points);
    }
  }
}

void DatapathView::calculateLayout() {
    // Layout the datapath components in a logical flow (left to right)
    m_pcBox.position = {m_x, m_y + 400.f};
    m_pcBox.size = {60.f, 40.f};
    m_pcBox.label = "PC";

}

void DatapathView::setupWires() {
    m_wires.clear();
}

void DatapathView::update() {
    // Update will be used to highlight active paths based on current instruction
}

void DatapathView::draw(sf::RenderWindow& window) {
    // Draw wires first (so they appear behind components)
    // for (const auto& wire : m_wires) {
    //     drawWire(window, wire);
    // }
    
    // Draw all major components
    drawComponentBox(window, m_pcBox, sf::Color(100, 150, 200));
    
    // Draw wire labels on top of everything
    // for (const auto& wire : m_wires) {
    //     if (!wire.label.empty()) {
    //         drawWireLabel(window, wire);
    //     }
    // }
}

void DatapathView::drawComponentBox(sf::RenderWindow& window, const ComponentBox& box, sf::Color color) {
    // Draw component rectangle
    sf::RectangleShape rect(box.size);
    rect.setPosition(box.position);
    rect.setFillColor(color);
    rect.setOutlineColor(sf::Color::White);
    rect.setOutlineThickness(2.f);
    window.draw(rect);
    
    // Draw label
    sf::Text label(m_font);
    label.setString(box.label);
    label.setCharacterSize(12);
    label.setFillColor(sf::Color::White);
    
    // Center the label in the box
    sf::FloatRect textBounds = label.getLocalBounds();
    label.setOrigin({
        textBounds.position.x + textBounds.size.x / 2.0f,
        textBounds.position.y + textBounds.size.y / 2.0f
    });
    label.setPosition({
        box.position.x + box.size.x / 2.0f,
        box.position.y + box.size.y / 2.0f
    });
    window.draw(label);
}

void DatapathView::drawWire(sf::RenderWindow& window, const Wire& wire) {
    // Determine color based on active state
    sf::Color lineColor = wire.active ? sf::Color::Yellow : wire.color;
    
    // Draw the line
    sf::Vertex line[] = {
        sf::Vertex{{wire.start.x, wire.start.y}, lineColor},
        sf::Vertex{{wire.end.x, wire.end.y}, lineColor}
    };
    
    // Make active wires thicker by drawing multiple lines
    if (wire.active) {
        sf::Vertex thickLine1[] = {
            sf::Vertex{{wire.start.x + 1.f, wire.start.y}, lineColor},
            sf::Vertex{{wire.end.x + 1.f, wire.end.y}, lineColor}
        };
        sf::Vertex thickLine2[] = {
            sf::Vertex{{wire.start.x, wire.start.y + 1.f}, lineColor},
            sf::Vertex{{wire.end.x, wire.end.y + 1.f}, lineColor}
        };
        window.draw(thickLine1, 2, sf::PrimitiveType::Lines);
        window.draw(thickLine2, 2, sf::PrimitiveType::Lines);
    }
    
    window.draw(line, 2, sf::PrimitiveType::Lines);
    
    // Draw arrowhead at end
    sf::CircleShape arrow(wire.active ? 4.f : 3.f);
    arrow.setOrigin({arrow.getRadius(), arrow.getRadius()});
    arrow.setPosition(wire.end);
    arrow.setFillColor(lineColor);
    window.draw(arrow);
}

void DatapathView::drawWireLabel(sf::RenderWindow& window, const Wire& wire) {
    if (wire.label.empty()) return;
    
    // Calculate midpoint of wire
    sf::Vector2f midpoint = {
        (wire.start.x + wire.end.x) / 2.f,
        (wire.start.y + wire.end.y) / 2.f
    };
    
    // Create label text
    sf::Text label(m_font);
    label.setString(wire.label);
    label.setCharacterSize(10);
    label.setFillColor(sf::Color::White);
    
    // Create background box for label
    sf::FloatRect textBounds = label.getLocalBounds();
    sf::RectangleShape background({
        textBounds.size.x + 6.f,
        textBounds.size.y + 6.f
    });
    background.setPosition({
        midpoint.x - textBounds.size.x / 2.f - 3.f,
        midpoint.y - textBounds.size.y / 2.f - 3.f
    });
    background.setFillColor(sf::Color(40, 40, 50, 220));  // Semi-transparent
    background.setOutlineColor(wire.color);
    background.setOutlineThickness(1.f);
    
    // Position label at midpoint
    label.setPosition({
        midpoint.x - textBounds.size.x / 2.f,
        midpoint.y - textBounds.size.y / 2.f
    });
    
    window.draw(background);
    window.draw(label);
}

} // namespace ez_arch
