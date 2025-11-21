#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <string>

#include "gui/style.hpp"

namespace ez_arch {

// Thanks to SFML docs for this!
class EllipseShape : public sf::Shape {
public:
  explicit EllipseShape(sf::Vector2f radius = {0, 0}) : m_radius(radius) {
    update();
  }

  void set_radius(sf::Vector2f radius) {
    m_radius = radius;
    update();
  }

  void set_label(const std::string& label) { m_label = label; }

  std::string& get_label() { return m_label; }

  sf::Vector2f get_radius() const { return m_radius; }

  std::size_t getPointCount() const override {
    return 30;
  }  // fixed, but could be an attribute of the class if needed

  sf::Vector2f getPoint(std::size_t index) const override {
    static constexpr float kPI = 3.141592654F;

    float angle = (index * 2 * kPI / getPointCount()) - (kPI / 2);
    float x = std::cos(angle) * m_radius.x;
    float y = std::sin(angle) * m_radius.y;

    return m_radius + sf::Vector2f(x, y);
  }

 private:
  sf::Vector2f m_radius;
  std::string m_label;
};

class ALUShape : public sf::Drawable, public sf::Transformable {
 public:
  explicit ALUShape(sf::Font& font, bool is_data_alu = false,
           unsigned int character_size = 12, sf::Vector2f origin = {0, 0},
           sf::Color color = sf::Color::White,
           sf::Color outline_color = sf::Color::Black)
      : m_font(font),
        m_vertices(sf::PrimitiveType::Triangles, 15),
        m_fillColor(color),
        m_outlineVertices(sf::PrimitiveType::LineStrip, 8),
        m_outlineColor(outline_color),
        m_dataALU(is_data_alu),
        m_characterSize(character_size),
        m_aluScale(sf::Vector2f(1.F, 1.F)) {
    Transformable::setOrigin(origin);
    update_position();
    update_fill_color();
    update_outline_color();
  }

  void set_font(sf::Font& font) { m_font = font; }

  void set_label(const std::string& label) { m_label = label; }

  void set_character_size(unsigned int character_size) {
    m_characterSize = character_size;
  }

  void set_fill_color(const sf::Color kCOLOR) {
    m_fillColor = kCOLOR;
    update_fill_color();
  }

  void set_outline_color(const sf::Color kCOLOR) {
    m_outlineColor = kCOLOR;
    update_outline_color();
  }

  void set_alu_scale(sf::Vector2f scale) {
    m_aluScale = scale;
    update_position();
  }

  sf::Vector2f get_alu_scale() { return m_aluScale; }

  void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
    states.transform *= getTransform();
    target.draw(m_vertices, states);
    target.draw(m_outlineVertices, states);

    sf::Vector2f position = Transformable::getPosition();
    sf::Text label(m_font, m_label, m_characterSize);
    label.setFillColor(sf::Color::Black);
    label.setPosition(sf::Vector2f(position.x + m_vertices[14].position.x + 2.F,
                                   position.y + m_vertices[14].position.y));
    sf::FloatRect text_bounds = label.getLocalBounds();
    label.setOrigin({0, text_bounds.position.y + (text_bounds.size.y / 2.0F)});
    target.draw(label);

    if (m_dataALU) {
      sf::Text zero(m_font, "Zero", m_characterSize);
      zero.setFillColor(sf::Color::Black);
      zero.setPosition(
          sf::Vector2f(position.x + m_vertices[2].position.x,
                       position.y + m_vertices[2].position.y + 20.F));
      sf::FloatRect zero_bounds = zero.getLocalBounds();
      zero.setOrigin({zero_bounds.position.x + zero_bounds.size.x + 2.F, 0});
      target.draw(zero);

      sf::Text result(m_font, "Result", m_characterSize);
      result.setFillColor(sf::Color::Black);
      result.setPosition(
          sf::Vector2f(position.x + m_vertices[8].position.x,
                       position.y + m_vertices[8].position.y - 30.F));
      sf::FloatRect result_bounds = result.getLocalBounds();
      result.setOrigin({result_bounds.position.x + result_bounds.size.x + 2.F,
                        result_bounds.position.y + result_bounds.size.y});
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

  void update_fill_color() {
    for (size_t i = 0; i < 12; i += 3) {
      m_vertices[i].color = m_fillColor;
      m_vertices[i + 1].color = m_fillColor;
      m_vertices[i + 2].color = m_fillColor;
    }
    m_vertices[12].color = kDATA_PATH_BACKGROUND_COLOR;
    m_vertices[13].color = kDATA_PATH_BACKGROUND_COLOR;
    m_vertices[14].color = kDATA_PATH_BACKGROUND_COLOR;
  }

  void update_outline_color() {
    for (size_t i = 0; i < 8; ++i) {
      m_outlineVertices[i].color = m_outlineColor;
    }
  }

  void update_position() {
    m_vertices[0].position =
        sf::Vector2f(0.F * m_aluScale.x, 0.F * m_aluScale.y);
    m_vertices[1].position =
        sf::Vector2f(0.F * m_aluScale.x, 20.F * m_aluScale.y);
    m_vertices[2].position =
        sf::Vector2f(50.F * m_aluScale.x, 20.F * m_aluScale.y);

    m_vertices[3].position =
        sf::Vector2f(50.F * m_aluScale.x, 20.F * m_aluScale.y);
    m_vertices[4].position =
        sf::Vector2f(0.F * m_aluScale.x, 20.F * m_aluScale.y);
    m_vertices[5].position =
        sf::Vector2f(0.F * m_aluScale.x, 80.F * m_aluScale.y);

    m_vertices[6].position =
        sf::Vector2f(0.F * m_aluScale.x, 80.F * m_aluScale.y);
    m_vertices[7].position =
        sf::Vector2f(50.F * m_aluScale.x, 20.F * m_aluScale.y);
    m_vertices[8].position =
        sf::Vector2f(50.F * m_aluScale.x, 80.F * m_aluScale.y);

    m_vertices[9].position =
        sf::Vector2f(50.F * m_aluScale.x, 80.F * m_aluScale.y);
    m_vertices[10].position =
        sf::Vector2f(0.F * m_aluScale.x, 100.F * m_aluScale.y);
    m_vertices[11].position =
        sf::Vector2f(0.F * m_aluScale.x, 80.F * m_aluScale.y);

    m_vertices[12].position =
        sf::Vector2f(0.F * m_aluScale.x, 40.F * m_aluScale.y);
    m_vertices[13].position =
        sf::Vector2f(0.F * m_aluScale.x, 60.F * m_aluScale.y);
    m_vertices[14].position =
        sf::Vector2f(10.F * m_aluScale.x, 50.F * m_aluScale.y);

    m_outlineVertices[0].position =
        sf::Vector2f(0.F * m_aluScale.x, 0.F * m_aluScale.y);
    m_outlineVertices[1].position =
        sf::Vector2f(50.F * m_aluScale.x, 20.F * m_aluScale.y);
    m_outlineVertices[2].position =
        sf::Vector2f(50.F * m_aluScale.x, 80.F * m_aluScale.y);
    m_outlineVertices[3].position =
        sf::Vector2f(0.F * m_aluScale.x, 100.F * m_aluScale.y);
    m_outlineVertices[4].position =
        sf::Vector2f(0.F * m_aluScale.x, 60.F * m_aluScale.y);
    m_outlineVertices[5].position =
        sf::Vector2f(10.F * m_aluScale.x, 50.F * m_aluScale.y);
    m_outlineVertices[6].position =
        sf::Vector2f(0.F * m_aluScale.x, 40.F * m_aluScale.y);
    m_outlineVertices[7].position =
        sf::Vector2f(0.F * m_aluScale.x, 0.F * m_aluScale.y);
  }
};

class MuxShape : public sf::Drawable, public sf::Transformable {
 public:
  explicit MuxShape(sf::Font& font, bool flip = false, unsigned int character_size = 12,
           sf::Vector2f origin = {0, 0}, sf::Color fill_color = sf::Color::White,
           sf::Color outline_color = sf::Color::Black)
      : m_font(font),
        m_flip(flip),
        m_characterSize(character_size),
        m_fillColor(fill_color),
        m_outlineColor(outline_color),
        m_topCircle(15),
        m_bottomCircle(15),
        m_centerRect(sf::Vector2f(30.F, 64.F)),
        m_outlineVertices(sf::PrimitiveType::Lines, 4) {
    Transformable::setOrigin(origin);
    update_position();
    update_fill_color();
    update_outline_color();
  }

  void set_font(sf::Font& font) { m_font = font; }

  void flip() { m_flip = !m_flip; }

  void set_character_size(unsigned int character_size) {
    m_characterSize = character_size;
  }

  void set_outline_color(sf::Color outline_color) {
    m_outlineColor = outline_color;
    update_outline_color();
  }

  void set_fill_color(sf::Color fill_color) {
    m_fillColor = fill_color;
    update_fill_color();
  }

  void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
    states.transform *= getTransform();
    target.draw(m_topCircle, states);
    target.draw(m_bottomCircle, states);
    target.draw(m_centerRect, states);
    target.draw(m_outlineVertices, states);

    std::string top_label = m_flip ? "1" : "0";
    sf::Vector2f position = Transformable::getPosition();
    sf::Text top(m_font, top_label, m_characterSize);
    top.setFillColor(sf::Color::Black);
    top.setPosition(sf::Vector2f(position.x + 15.F, position.y + 15.F));
    sf::FloatRect text_bounds = top.getLocalBounds();
    top.setOrigin({text_bounds.position.x + (text_bounds.size.x / 2.F),
                   text_bounds.position.y + (text_bounds.size.y / 2.F)});
    target.draw(top);

    std::string bottom_label = m_flip ? "0" : "1";
    sf::Text bottom(m_font, bottom_label, m_characterSize);
    bottom.setFillColor(sf::Color::Black);
    bottom.setPosition(sf::Vector2f(position.x + 15.F, position.y + 80.F));
    bottom.setOrigin({text_bounds.position.x + (text_bounds.size.x / 2.F),
                      text_bounds.position.y + (text_bounds.size.y / 2.F)});
    target.draw(bottom);

    sf::Text center(m_font, "M\nU\nX", m_characterSize);
    center.setFillColor(sf::Color::Black);
    center.setPosition(sf::Vector2f(position.x + 14.F, position.y + 33.F));
    center.setOrigin({text_bounds.position.x + (text_bounds.size.x / 2.F),
                      text_bounds.position.y + (text_bounds.size.y / 2.F)});
    target.draw(center);
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

  void update_fill_color() {
    m_topCircle.setFillColor(m_fillColor);
    m_topCircle.setOutlineThickness(1.F);
    m_bottomCircle.setFillColor(m_fillColor);
    m_bottomCircle.setOutlineThickness(1.F);
    m_centerRect.setFillColor(m_fillColor);
  }

  void update_outline_color() {
    m_topCircle.setOutlineColor(m_outlineColor);
    m_bottomCircle.setOutlineColor(m_outlineColor);

    for (size_t i = 0; i < 4; ++i) {
      m_outlineVertices[i].color = m_outlineColor;
    }
  }

  void update_position() {
    m_topCircle.setPosition(sf::Vector2f(0, 0));
    m_bottomCircle.setPosition(sf::Vector2f(0, 60.F));
    m_centerRect.setPosition(sf::Vector2f(0, 16.F));

    m_outlineVertices[0].position = sf::Vector2f(0, 18.F);
    m_outlineVertices[1].position = sf::Vector2f(0, 80.F);
    m_outlineVertices[2].position = sf::Vector2f(30.F, 18.F);
    m_outlineVertices[3].position = sf::Vector2f(30.F, 80.F);
  }
};

class AndGateShape : public sf::Drawable, public sf::Transformable {
 public:
  explicit AndGateShape(sf::Color fill_color = sf::Color::White,
               sf::Color outline_color = sf::Color::Black)
      : m_fillColor(fill_color),
        m_outlineColor(outline_color),
        m_circle(25.F),
        m_rect(sf::Vector2f(20.F, 50.F)),
        m_outlineVertices(sf::PrimitiveType::Lines, 6) {
    update_outline_color();
    update_fill_color();
    update_position();
  }

  void set_fill_color(sf::Color fill_color) {
    m_fillColor = fill_color;
    update_fill_color();
  }

  void set_outline_color(sf::Color outline_color) {
    m_outlineColor = outline_color;
    update_outline_color();
  }

  void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
    states.transform *= getTransform();
    target.draw(m_circle, states);
    target.draw(m_rect, states);
    target.draw(m_outlineVertices, states);
  }

 private:
  sf::Color m_fillColor;
  sf::Color m_outlineColor;
  sf::CircleShape m_circle;
  sf::RectangleShape m_rect;
  sf::VertexArray m_outlineVertices;
  // sf::Vector2f m_topInput  // For wires in the future
  // sf::Vector2f m_bottomInput; // For wires in the future

  void update_fill_color() {
    m_circle.setFillColor(m_fillColor);
    m_rect.setFillColor(m_fillColor);
  }

  void update_outline_color() {
    m_circle.setOutlineColor(m_outlineColor);
    m_circle.setOutlineThickness(1.F);

    for (size_t i = 0; i < 6; ++i) {
      m_outlineVertices[i].color = m_outlineColor;
    }
  }

  void update_position() {
    m_circle.setPosition(sf::Vector2f(0, 0));
    m_rect.setPosition(sf::Vector2f(0, 0));

    m_outlineVertices[0].position = sf::Vector2f(25.F, 0);
    m_outlineVertices[1].position = sf::Vector2f(0, 0);
    m_outlineVertices[2].position = sf::Vector2f(0, 0);
    m_outlineVertices[3].position = sf::Vector2f(0, 50.F);
    m_outlineVertices[4].position = sf::Vector2f(0, 51.F);
    m_outlineVertices[5].position = sf::Vector2f(25.F, 51.F);
  }
};

// TODO(evice): Create another shape for lines with lables.

}  // namespace ez_arch
