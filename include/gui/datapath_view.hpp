#pragma once

#include "core/cpu.hpp"
#include "gui/custom_shapes.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <memory>
#include <string>
#include <vector>

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
  std::string m_currentWiresInstruction;
  float m_x {0.F};
  float m_y {0.F};
  float m_width {800.F};
  float m_height {600.F};

  // Component positions (calculated based on size)
  struct ComponentBox {
    sf::Vector2f position;
    sf::Vector2f size;
    std::string label;
    std::vector<std::string> inputs;
    std::array<std::string, 4> outputs;

    // Helper to get connection points
    [[nodiscard]] sf::Vector2f getLeft(size_t inputPos,
                                       float characterSize = 12) const {
      auto yOffsetScale = static_cast<float>(
          ((inputPos * 2) + 1) / static_cast<float>(inputs.size() * 2));
      return {position.x + 5.F,
              position.y + (size.y * yOffsetScale - characterSize)};
    }
    [[nodiscard]] sf::Vector2f getRight(size_t outputPos,
                                        float characterSize = 12,
                                        float xOffset = 0) const {
      auto yOffsetScale =
          static_cast<float>(((outputPos * 2) + 1) / static_cast<float>(4 * 2));
      return {position.x + (size.x / 2.F) + xOffset,
              position.y + (size.y * yOffsetScale - characterSize)};
    }
    [[nodiscard]] sf::Vector2f getTop() const {
      return {position.x + (size.x / 2.F), position.y};
    }
    [[nodiscard]] sf::Vector2f getBottom() const {
      return {position.x + (size.x / 2.F), position.y + size.y};
    }
    [[nodiscard]] sf::Vector2f getCenter() const {
      return {position.x + (size.x / 2.F), position.y + (size.y / 2.F)};
    }
  };

  // Wire/Connection definition
  struct Wire {
    Wire() {
      vertices =
          std::make_unique<sf::VertexArray>(sf::PrimitiveType::LineStrip, 3);
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
      if (this == &other) { return *this; }
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
    bool active = true; // Highlight when active
    bool no_arrow = false;
  };

  struct WireLabel {
    size_t wireIndex = kNUMBER_OF_WIRES - 1;
    std::string label;
    bool valid = false;
    sf::Vector2f position;

    [[nodiscard]] const Wire& getWire(const DatapathView& view) const {
      return view.m_wires[wireIndex];
    }
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
  static constexpr size_t kNUMBER_OF_WIRES = 43;
  static constexpr size_t kNUMBER_OF_WIRE_LABLES = 23;
  std::array<Wire, kNUMBER_OF_WIRES>
      m_wires {}; // Last wire is "invalid" and used as a temp wire for labels
  std::array<WireLabel, kNUMBER_OF_WIRE_LABLES> m_wireLabels {};

  // Drawing helpers
  void calculateLayout();
  void setupWires();
  void setupWireLabels();
  void updateWirePosition(float x, float y);
  void updateWireLabelPosition(float x, float y);
  void drawComponentBox(sf::RenderWindow& window,
                        const ComponentBox& box,
                        sf::Color color);
  void drawEllipse(sf::RenderWindow& window,
                   std::unique_ptr<EllipseShape>& circle,
                   sf::Color color);
  static void drawAlu(sf::RenderWindow& window,
                      std::unique_ptr<ALUShape>& alu,
                      sf::Color color = sf::Color::White);
  static void drawMux(sf::RenderWindow& window,
                      std::unique_ptr<MuxShape>& mux,
                      sf::Color color = sf::Color::White);
  static void drawGate(sf::RenderWindow& window,
                       std::unique_ptr<AndGateShape>& gate,
                       sf::Color color = sf::Color::White);
  void drawWire(sf::RenderWindow& window, Wire& wire);
  void drawLabel(sf::RenderWindow& window, WireLabel& wireLable);
  void turnOffWires(const std::string& instructionName);
};

} // namespace ez_arch
