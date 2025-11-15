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
    ALUShape(sf::Font& font, bool isDataALU = false, unsigned int characterSize = 12, sf::Vector2f origin = {0,0},
        sf::Color color = sf::Color::White, sf::Color outlineColor = sf::Color::Black)
      : m_font(font), m_vertices(sf::PrimitiveType::Triangles, 15), m_fillColor(color),
        m_outlineVertices(sf::PrimitiveType::LineStrip, 8), m_outlineColor(outlineColor),
        m_dataALU(isDataALU), m_characterSize(characterSize), m_aluScale(sf::Vector2f(1.f, 1.f)) {
      Transformable::setOrigin(origin);
      updatePosition();
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

    void setAluScale(sf::Vector2f scale) {
      m_aluScale = scale;
      updatePosition();
    }

    sf::Vector2f getAluScale() {
      return m_aluScale;
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
      states.transform *= getTransform();
      target.draw(m_vertices, states);
      target.draw(m_outlineVertices, states);
      
      sf::Vector2f position = Transformable::getPosition();
      sf::Text label(m_font, m_label, m_characterSize);
      label.setFillColor(sf::Color::Black);
      label.setPosition(sf::Vector2f(
        position.x + m_vertices[14].position.x + 2.f,
        position.y + m_vertices[14].position.y
      ));
      sf::FloatRect textBounds = label.getLocalBounds();
      label.setOrigin({ 0, textBounds.position.y + textBounds.size.y / 2.0f});
      target.draw(label);

      if (m_dataALU) {
        sf::Text zero(m_font, "Zero", m_characterSize);
        zero.setFillColor(sf::Color::Black);
        zero.setPosition(sf::Vector2f(
          position.x + m_vertices[2].position.x,
          position.y + m_vertices[2].position.y + 20.f
        ));
        sf::FloatRect zeroBounds = zero.getLocalBounds();
        zero.setOrigin({ zeroBounds.position.x + zeroBounds.size.x + 2.f, 0});
        target.draw(zero);

        sf::Text result(m_font, "Result", m_characterSize);
        result.setFillColor(sf::Color::Black);
        result.setPosition(sf::Vector2f(
          position.x + m_vertices[8].position.x,
          position.y + m_vertices[8].position.y - 30.f
        ));
        sf::FloatRect resultBounds = result.getLocalBounds();
        result.setOrigin({ resultBounds.position.x + resultBounds.size.x + 2.f, resultBounds.position.y + resultBounds.size.y});
        target.draw(result);
      }
    }

private:
    sf::Font& m_font;
    sf::VertexArray m_vertices;
    sf::Color m_fillColor;
    sf::VertexArray m_outlineVertices;
    sf::Color m_outlineColor;
    std::string m_label;
    bool m_dataALU;
    unsigned int m_characterSize;
    sf::Vector2f m_aluScale;


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

    void updatePosition() {
      m_vertices[0].position = sf::Vector2f(0.f * m_aluScale.x, 0.f * m_aluScale.y);
      m_vertices[1].position = sf::Vector2f(0.f * m_aluScale.x, 20.f * m_aluScale.y);
      m_vertices[2].position = sf::Vector2f(50.f * m_aluScale.x, 20.f * m_aluScale.y);

      m_vertices[3].position = sf::Vector2f(50.f * m_aluScale.x, 20.f * m_aluScale.y);
      m_vertices[4].position = sf::Vector2f(0.f * m_aluScale.x, 20.f * m_aluScale.y);
      m_vertices[5].position = sf::Vector2f(0.f * m_aluScale.x, 80.f * m_aluScale.y);

      m_vertices[6].position = sf::Vector2f(0.f * m_aluScale.x, 80.f * m_aluScale.y);
      m_vertices[7].position = sf::Vector2f(50.f * m_aluScale.x, 20.f * m_aluScale.y);
      m_vertices[8].position = sf::Vector2f(50.f * m_aluScale.x, 80.f * m_aluScale.y);

      m_vertices[9].position = sf::Vector2f(50.f * m_aluScale.x, 80.f * m_aluScale.y);
      m_vertices[10].position = sf::Vector2f(0.f * m_aluScale.x, 100.f * m_aluScale.y);
      m_vertices[11].position = sf::Vector2f(0.f * m_aluScale.x, 80.f * m_aluScale.y);

      m_vertices[12].position = sf::Vector2f(0.f * m_aluScale.x, 40.f * m_aluScale.y);
      m_vertices[13].position = sf::Vector2f(0.f * m_aluScale.x, 60.f * m_aluScale.y);
      m_vertices[14].position = sf::Vector2f(10.f * m_aluScale.x, 50.f * m_aluScale.y);


      m_outlineVertices[0].position = sf::Vector2f(0.f * m_aluScale.x, 0.f * m_aluScale.y);
      m_outlineVertices[1].position = sf::Vector2f(50.f * m_aluScale.x, 20.f * m_aluScale.y);
      m_outlineVertices[2].position = sf::Vector2f(50.f * m_aluScale.x, 80.f * m_aluScale.y);
      m_outlineVertices[3].position = sf::Vector2f(0.f * m_aluScale.x, 100.f * m_aluScale.y);
      m_outlineVertices[4].position = sf::Vector2f(0.f * m_aluScale.x, 60.f * m_aluScale.y);
      m_outlineVertices[5].position = sf::Vector2f(10.f * m_aluScale.x, 50.f * m_aluScale.y);
      m_outlineVertices[6].position = sf::Vector2f(0.f * m_aluScale.x, 40.f * m_aluScale.y);
      m_outlineVertices[7].position = sf::Vector2f(0.f * m_aluScale.x, 0.f * m_aluScale.y);
    }
};

class MuxShape : public sf::Drawable, public sf::Transformable {
public:
  MuxShape( sf::Font& font, bool flip = 0, unsigned int characterSize = 12, sf::Vector2f origin = {0,0},
            sf::Color fillColor = sf::Color::White, sf::Color outlineColor = sf::Color::Black) :
    m_font(font), m_flip(flip), m_characterSize(characterSize),
    m_fillColor(fillColor), m_outlineColor(outlineColor), m_topCircle(20), m_bottomCircle(20),
    m_centerRect(sf::Vector2f(40, 60)), m_outlineVertices(sf::PrimitiveType::Lines, 4) {
      Transformable::setOrigin(origin);
      updatePosition();
      updateFillColor();
      updateOutlineColor();
    }

  void setFont(sf::Font& font) {
    m_font = font;
  }

  void flip() {
    m_flip = !m_flip;
  }

  void setCharacterSize(unsigned int characterSize) {
    m_characterSize = characterSize;
  }

  void setOutlineColor(sf::Color outlineColor) {
    m_outlineColor = outlineColor;
    updateOutlineColor();
  }
  
  void setFillColor(sf::Color fillColor) {
    m_fillColor = fillColor;
    updateFillColor();
  }

  void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
    states.transform *= getTransform();
    target.draw(m_topCircle, states);
    target.draw(m_bottomCircle, states);
    target.draw(m_centerRect, states);
    target.draw(m_outlineVertices, states);
  }

private:
  sf::Font& m_font;
  bool m_flip;
  unsigned int m_characterSize;
  sf::Color m_fillColor;
  sf::Color m_outlineColor;
  sf::CircleShape m_topCircle;
  sf::CircleShape m_bottomCircle;
  sf::RectangleShape m_centerRect;
  sf::VertexArray m_outlineVertices;

  void updateFillColor() {
    m_topCircle.setFillColor(m_fillColor);
    m_topCircle.setOutlineThickness(1.f);
    m_bottomCircle.setFillColor(m_fillColor);
    m_bottomCircle.setOutlineThickness(1.f);
    m_centerRect.setFillColor(m_fillColor);
  }

  void updateOutlineColor() {
    m_topCircle.setOutlineColor(m_outlineColor);
    m_bottomCircle.setOutlineColor(m_outlineColor);
    
    for (size_t i = 0; i < 4; ++i) {
      m_outlineVertices[i].color = m_outlineColor;
    }
  }

  void updatePosition() {
    m_topCircle.setPosition(sf::Vector2f(0,0));
    m_bottomCircle.setPosition(sf::Vector2f(0, 60.f));
    m_centerRect.setPosition(sf::Vector2f(0, 20.f));

    m_outlineVertices[0].position = sf::Vector2f(0, 20.f);
    m_outlineVertices[1].position = sf::Vector2f(0, 80.f);
    m_outlineVertices[2].position = sf::Vector2f(41.f, 20.f);
    m_outlineVertices[3].position = sf::Vector2f(41.f, 80.f);
  }

};

// TODO: Create another shape for lines with lables.

} // namesape ez_arch
