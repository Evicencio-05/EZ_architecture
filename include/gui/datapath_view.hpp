#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <memory>
#include <vector>

#include "core/cpu.hpp"
#include "gui/custom_shapes.hpp"

namespace ez_arch {

class DatapathView {
 public:
  DatapathView(const CPU& cpu, sf::Font& font);

  void setPosition(float x, float y);
  void setSize(float width, float height);

  void update();
  void draw(sf::RenderWindow& window);

  void drawScaffolding(sf::RenderWindow& window, sf::Vector2f mainAreaSize);

 private:
  const CPU& m_cpu;
  sf::Font& m_font;
  float m_x;
  float m_y;
  float m_width;
  float m_height;

  // Component positions (calculated based on size)
  struct ComponentBox {
    sf::Vector2f position;
    sf::Vector2f size;
    std::string label;
    std::vector<std::string> inputs;
    std::array<std::string, 4> outputs;

    // Helper to get connection points
    sf::Vector2f getLeft(size_t inputPos, float characterSize = 12) const {
      float yOffsetScale = static_cast<float>(
          ((inputPos * 2) + 1) / static_cast<float>(inputs.size() * 2));
      return {position.x + 5.f,
              position.y + (size.y * yOffsetScale - characterSize)};
    }
    sf::Vector2f getRight(size_t outputPos, float characterSize = 12,
                          float xOffset = 0) const {
      float yOffsetScale =
          static_cast<float>(((outputPos * 2) + 1) / static_cast<float>(4 * 2));
      return {position.x + (size.x / 2.f) + xOffset,
              position.y + (size.y * yOffsetScale - characterSize)};
    }
    sf::Vector2f getTop() const {
      return {position.x + size.x / 2.f, position.y};
    }
    sf::Vector2f getBottom() const {
      return {position.x + size.x / 2.f, position.y + size.y};
    }
    sf::Vector2f getCenter() const {
      return {position.x + size.x / 2.f, position.y + size.y / 2.f};
    }
  };

  // Wire/Connection definition
  struct Wire {
    Wire() {
      vertices = std::make_unique<sf::VertexArray>(sf::PrimitiveType::LineStrip, 3);
      vertices->clear();
    }

    Wire(const Wire& other)
        : number(other.number),
          color(other.color),
          active(other.active),
          no_arrow(other.no_arrow) {
      if (other.vertices) {
        vertices = std::make_unique<sf::VertexArray>(*other.vertices);
      }
    }

    Wire& operator=(const Wire& other) {
      if (this == &other) return *this;
      number = other.number;
      color = other.color;
      active = other.active;
      no_arrow = other.no_arrow;
      vertices = other.vertices
                     ? std::make_unique<sf::VertexArray>(*other.vertices)
                     : nullptr;
      return *this;
    }

    Wire(Wire&&) noexcept = default;
    Wire& operator=(Wire&&) noexcept = default;

    std::unique_ptr<sf::VertexArray> vertices;
    std::string number;
    sf::Color color = sf::Color::Black;
    bool active = true;  // Highlight when active
    bool no_arrow = false;
  };

  struct WireLabel {
    size_t wireIndex = 42;
    std::string label;
    bool valid = false;

    const Wire& getWire() const { return m_wires[wireIndex]; }
  };

  ComponentBox m_pcBox;
  ComponentBox m_instructionMemory;
  ComponentBox m_registers;
  ComponentBox m_dataMemory;

  std::unique_ptr<EllipseShape> m_control;
  std::unique_ptr<EllipseShape> m_signExt;
  std::unique_ptr<EllipseShape> m_ALUControl;
  std::unique_ptr<EllipseShape> m_jumpSL;
  std::unique_ptr<EllipseShape> m_branchSL;

  std::unique_ptr<ALUShape> m_pcAlu;
  std::unique_ptr<ALUShape> m_branchAlu;
  std::unique_ptr<ALUShape> m_dataAlu;

  std::unique_ptr<MuxShape> m_regMux;
  std::unique_ptr<MuxShape> m_dataMux;
  std::unique_ptr<MuxShape> m_branchMux;
  std::unique_ptr<MuxShape> m_jumpMux;
  std::unique_ptr<MuxShape> m_writeMux;

  std::unique_ptr<AndGateShape> m_andGate;

  // All wire connections
  std::array<Wire, 43> m_wires{}; // Last wire is "invalid" and used as a temp wire for labels
  std::array<WireLabel, 25> m_wireLabels {};

  // Drawing helpers
  void calculateLayout();
  void setupWires();
  void setupWireLabels();
  void updateWirePosition(float x, float y);
  void drawComponentBox(sf::RenderWindow& window, const ComponentBox& box,
                        sf::Color color);
  void drawEllipse(sf::RenderWindow& window,
                   std::unique_ptr<EllipseShape>& circle, sf::Color color);
  void drawALU(sf::RenderWindow& window, std::unique_ptr<ALUShape>& alu,
               sf::Color color = sf::Color::White);
  void drawMux(sf::RenderWindow& window, std::unique_ptr<MuxShape>& mux,
               sf::Color color = sf::Color::White);
  void drawGate(sf::RenderWindow& window, std::unique_ptr<AndGateShape>& gate,
                sf::Color color = sf::Color::White);
  void drawWire(sf::RenderWindow& window, Wire& wire);
  void drawLabel(sf::RenderWindow& window, WireLabel& wireLable);
  // TODO: Create a drawLabel function. Connect to wrires to automatically turn
  // off when needed
};

}  // namespace ez_arch
