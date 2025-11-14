#pragma once

#include <cmath>
#include <string>
#include <SFML/Graphics.hpp>
#include "gui/style.hpp"

namespace ez_arch {

// Thanks to SFML docs for this!
class EllipseShape : public sf::Shape
{
public:
    explicit EllipseShape(sf::Vector2f radius = {0, 0}) : m_radius(radius) {
        update();
    }

    void setRadius(sf::Vector2f radius) {
        m_radius = radius;
        update();
    }

    void setLabel(const std::string& label) { m_label = label; }
    
    std::string& getLabel() { return m_label; }

    sf::Vector2f getRadius() const { return m_radius; }

    std::size_t getPointCount() const override { return 30; } // fixed, but could be an attribute of the class if needed

    sf::Vector2f getPoint(std::size_t index) const override
    {
        static constexpr float pi = 3.141592654f;

        float angle = index * 2 * pi / getPointCount() - pi / 2;
        float x     = std::cos(angle) * m_radius.x;
        float y     = std::sin(angle) * m_radius.y;

        return m_radius + sf::Vector2f(x, y);
    }

private:
    sf::Vector2f m_radius;
    std::string m_label;
};

class ALUShape : public sf::Drawable, public sf::Transformable {
public:
    ALUShape(sf::Font& font, unsigned int characterSize = 12, sf::Vector2f originPosition = sf::Vector2f(0.f,0.f), sf::Color color = sf::Color::White,
              sf::Color outlineColor = sf::Color::Black)
      : m_font(font), m_vertices(sf::PrimitiveType::Triangles, 15), m_fillColor(color), m_originPosition(originPosition),
        m_outlineVertices(sf::PrimitiveType::LineStrip, 8), m_outlineColor(outlineColor), m_characterSize(characterSize) {
      updatePositon();
      updateFillColor();
      updateOutlineColor();
    }

    void setFont(sf::Font& font) {
      m_font = font;
    }

    void setLabel(const std::string& label) {
      m_label = label;
    }

    void setCharacterSize(unsigned int characterSize) {
      m_characterSize = characterSize;
    }

    void setFillColor(const sf::Color color) {
      m_fillColor = color;
      updateFillColor();
    }

    void setOutlineColor(const sf::Color color) {
      m_outlineColor = color;
      updateOutlineColor();
    }

    void setOriginPosition(sf::Vector2f position) {
      m_originPosition = position;
      updatePositon();
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
      states.transform *= getTransform();
      target.draw(m_vertices, states);
      target.draw(m_outlineVertices, states);

      sf::Text label(m_font, m_label, m_characterSize);
      label.setFillColor(sf::Color::Black);
      label.setPosition(sf::Vector2f(m_vertices[14].position.x + 1.f, m_vertices[14].position.y));
      sf::FloatRect textBounds = label.getLocalBounds();
      label.setOrigin({ 0, textBounds.position.y + textBounds.size.y / 2.0f});
      target.draw(label);
    }

private:
    sf::Font& m_font;
    sf::VertexArray m_vertices;
    sf::Color m_fillColor;
    sf::Vector2f m_originPosition;
    sf::VertexArray m_outlineVertices;
    sf::Color m_outlineColor;
    std::string m_label;
    bool m_dataALU;
    unsigned int m_characterSize;


    void updateFillColor() {
      for (size_t i = 0; i < 12; i += 3) {
        m_vertices[i].color = m_fillColor;
        m_vertices[i + 1].color = m_fillColor;
        m_vertices[i + 2].color = m_fillColor;
      }
      m_vertices[12].color = DATA_PATH_BACKGROUND_COLOR;
      m_vertices[13].color = DATA_PATH_BACKGROUND_COLOR;
      m_vertices[14].color = DATA_PATH_BACKGROUND_COLOR;
    }

    void updateOutlineColor() {
      for (size_t i = 0; i < 8; ++i) {
        m_outlineVertices[i].color = m_outlineColor;
      }
    }

    void updatePositon() {
      m_vertices[0].position = sf::Vector2f(0.f + m_originPosition.x, 0.f + m_originPosition.y);
      m_vertices[1].position = sf::Vector2f(0.f + m_originPosition.x, 20.f + m_originPosition.y);
      m_vertices[2].position = sf::Vector2f(50.f + m_originPosition.x, 20.f + m_originPosition.y);

      m_vertices[3].position = sf::Vector2f(50.f + m_originPosition.x, 20.f + m_originPosition.y);
      m_vertices[4].position = sf::Vector2f(0.f + m_originPosition.x, 20.f + m_originPosition.y);
      m_vertices[5].position = sf::Vector2f(0.f + m_originPosition.x, 80.f + m_originPosition.y);

      m_vertices[6].position = sf::Vector2f(0.f + m_originPosition.x, 80.f + m_originPosition.y);
      m_vertices[7].position = sf::Vector2f(50.f + m_originPosition.x, 20.f + m_originPosition.y);
      m_vertices[8].position = sf::Vector2f(50.f + m_originPosition.x, 80.f + m_originPosition.y);

      m_vertices[9].position = sf::Vector2f(50.f + m_originPosition.x, 80.f + m_originPosition.y);
      m_vertices[10].position = sf::Vector2f(0.f + m_originPosition.x, 100.f + m_originPosition.y);
      m_vertices[11].position = sf::Vector2f(0.f + m_originPosition.x, 80.f + m_originPosition.y);

      m_vertices[12].position = sf::Vector2f(0.f + m_originPosition.x, 40.f + m_originPosition.y);
      m_vertices[13].position = sf::Vector2f(0.f + m_originPosition.x, 60.f + m_originPosition.y);
      m_vertices[14].position = sf::Vector2f(10.f + m_originPosition.x, 50.f + m_originPosition.y);


      m_outlineVertices[0].position = sf::Vector2f(0.f + m_originPosition.x, 0.f + m_originPosition.y);
      m_outlineVertices[1].position = sf::Vector2f(50.f + m_originPosition.x, 20.f + m_originPosition.y);
      m_outlineVertices[2].position = sf::Vector2f(50.f + m_originPosition.x, 80.f + m_originPosition.y);
      m_outlineVertices[3].position = sf::Vector2f(0.f + m_originPosition.x, 100.f + m_originPosition.y);
      m_outlineVertices[4].position = sf::Vector2f(0.f + m_originPosition.x, 60.f + m_originPosition.y);
      m_outlineVertices[5].position = sf::Vector2f(10.f + m_originPosition.x, 50.f + m_originPosition.y);
      m_outlineVertices[6].position = sf::Vector2f(0.f + m_originPosition.x, 40.f + m_originPosition.y);
      m_outlineVertices[7].position = sf::Vector2f(0.f + m_originPosition.x, 0.f + m_originPosition.y);
    }
};

} // namesape ez_arch
