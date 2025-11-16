#include "gui/datapath_view.hpp"

#include <SFML/System/Vector2.hpp>
#include <iostream>
#include <memory>
#include <string>

namespace ez_arch {

DatapathView::DatapathView(const CPU& cpu, sf::Font& font)
    : m_cpu(cpu),
      m_font(font),
      m_x(0.f),
      m_y(0.f),
      m_width(800.f),
      m_height(600.f) {
  m_control = std::make_unique<EllipseShape>();
  m_signExt = std::make_unique<EllipseShape>();
  m_ALUControl = std::make_unique<EllipseShape>();
  m_jumpSL = std::make_unique<EllipseShape>();
  m_branchSL = std::make_unique<EllipseShape>();

  m_pcAlu = std::make_unique<ALUShape>(m_font);
  m_branchAlu = std::make_unique<ALUShape>(m_font);
  m_dataAlu = std::make_unique<ALUShape>(m_font, true);

  m_regMux = std::make_unique<MuxShape>(m_font);
  m_dataMux = std::make_unique<MuxShape>(m_font);
  m_jumpMux = std::make_unique<MuxShape>(m_font, true);
  m_branchMux = std::make_unique<MuxShape>(m_font);
  m_writeMux = std::make_unique<MuxShape>(m_font, true);

  m_andGate = std::make_unique<AndGateShape>();

  calculateLayout();
  setupWires();

  m_pcBox.label = "PC";

  m_instructionMemory.label = "Instruction\n Memory";
  m_instructionMemory.inputs.push_back("Read\nAddress");
  m_instructionMemory.inputs.push_back("");
  m_instructionMemory.outputs[1] = "Instructions\n   [31-0]";

  m_registers.label = "\nRegisters";
  m_registers.inputs.push_back("Read\nRegister 1");
  m_registers.inputs.push_back("Read\nRegister 2");
  m_registers.inputs.push_back("Write\nRegister");
  m_registers.inputs.push_back("Write\nData");
  m_registers.outputs[0] = "        Read\n        Data 1";
  m_registers.outputs[2] = "        Read\n        Data 2";

  m_dataMemory.label = "     Data\n   Memory";
  m_dataMemory.inputs.push_back("");
  m_dataMemory.inputs.push_back("Address");
  m_dataMemory.inputs.push_back("");
  m_dataMemory.inputs.push_back("Write\nData");
  m_dataMemory.outputs[1] = "        Read\n        Data";

  m_control->setLabel("Control");

  m_signExt->setLabel(" Sign\nExtend");

  m_ALUControl->setLabel("  ALU\nControl");

  m_jumpSL->setLabel(" Shift\nLeft 2");

  m_branchSL->setLabel(" Shift\nLeft 2");

  m_pcAlu->setLabel("Add");

  m_branchAlu->setLabel("ALU\nResult");

  m_dataAlu->setLabel("ALU");
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

void DatapathView::drawScaffolding(sf::RenderWindow& window,
                                   sf::Vector2f mainAreaSize) {
  int width = static_cast<int>(mainAreaSize.y);
  int height = static_cast<int>(mainAreaSize.x);
  for (int i = 0; i < height; i += 50) {
    for (int j = 0; j < width; j += 50) {
      if (i == 0) {
        sf::Text text(m_font, std::to_string(j), 10);
        text.setFillColor(sf::Color::Black);
        text.setPosition(sf::Vector2f(static_cast<float>(i) + m_x,
                                      static_cast<float>(j) + m_y));
        window.draw(text);
      } else if (j == 0) {
        sf::Text text(m_font, std::to_string(i), 10);
        text.setFillColor(sf::Color::Black);
        text.setPosition(sf::Vector2f(static_cast<float>(i) + m_x,
                                      static_cast<float>(j) + m_y));
        window.draw(text);
      }
      sf::Vertex point{
          {static_cast<float>(i) + m_x, static_cast<float>(j) + m_y},
          sf::Color::Black};
      window.draw(&point, 1, sf::PrimitiveType::Points);
    }
  }
}

void DatapathView::calculateLayout() {
  // Layout the datapath components in a logical flow (left to right)
  m_pcBox.position = {m_x + 100.f, m_y + 500.f};
  m_pcBox.size = {50.f, 100.f};

  m_pcAlu->setPosition(sf::Vector2f(350.f + m_x, 75.f + m_y));

  m_instructionMemory.position = {m_x + 250.f, m_y + 500.f};
  m_instructionMemory.size = {150.f, 150.f};

  m_jumpSL->setRadius(sf::Vector2f(25.f, 25.f));
  m_jumpSL->setPosition(sf::Vector2f(m_x + 500.f, m_y + 50.f));

  m_regMux->setPosition(sf::Vector2f(650.f + m_x, 600.f + m_y));

  m_control->setRadius(sf::Vector2f(50.f, 125.f));
  m_control->setPosition(sf::Vector2f(800.f, 275.f));

  m_registers.position = {m_x + 750.f, m_y + 480.f};
  m_registers.size = {150.f, 250.f};

  m_signExt->setRadius(sf::Vector2f(50.f, 50.f));
  m_signExt->setPosition(sf::Vector2f(m_x + 800.f, m_y + 800.f));

  m_branchSL->setRadius(sf::Vector2f(25.f, 25.f));
  m_branchSL->setPosition(sf::Vector2f(m_x + 1000.f, m_y + 150.f));

  m_branchAlu->setPosition(sf::Vector2f(1100.f + m_x, 100.f + m_y));

  m_dataMux->setPosition(sf::Vector2f(1050.f + m_x, 600.f + m_y));

  m_dataAlu->setPosition(sf::Vector2f(1150.f + m_x, 500.f + m_y));
  m_dataAlu->setAluScale(sf::Vector2f(2.f, 2.f));

  m_ALUControl->setRadius(sf::Vector2f(50.f, 50.f));
  m_ALUControl->setPosition(sf::Vector2f(m_x + 1100.f, m_y + 800.f));

  m_branchMux->setPosition(sf::Vector2f(1350.f + m_x, 70.f + m_y));

  m_andGate->setPosition(sf::Vector2f(1300.f + m_x, 200.f + m_y));

  m_jumpMux->setPosition(sf::Vector2f(1450.f + m_x, 70.f + m_y));

  m_dataMemory.position = {m_x + 1350.f, m_y + 550.f};
  m_dataMemory.size = {150.f, 200.f};

  m_writeMux->setPosition(sf::Vector2f(1550.f + m_x, 610.f + m_y));
}

void DatapathView::setupWires() { m_wires.clear(); }

void DatapathView::update() {
  // Update will be used to highlight active paths based on current instruction
}

void DatapathView::draw(sf::RenderWindow& window) {
  // Draw wires first so they appear behind components
  // for (const auto& wire : m_wires) {
  //     drawWire(window, wire);
  // }

  // Draw all major components
  drawComponentBox(window, m_pcBox, sf::Color::White);
  drawComponentBox(window, m_instructionMemory, sf::Color::White);
  drawComponentBox(window, m_registers, sf::Color::White);
  drawComponentBox(window, m_dataMemory, sf::Color::White);

  drawEllipse(window, m_control, sf::Color::White);
  drawEllipse(window, m_signExt, sf::Color::White);
  drawEllipse(window, m_ALUControl, sf::Color::White);
  drawEllipse(window, m_jumpSL, sf::Color::White);
  drawEllipse(window, m_branchSL, sf::Color::White);

  drawALU(window, m_pcAlu);
  drawALU(window, m_branchAlu);
  drawALU(window, m_dataAlu);

  drawMux(window, m_regMux);
  drawMux(window, m_dataMux);
  drawMux(window, m_branchMux);
  drawMux(window, m_jumpMux);
  drawMux(window, m_writeMux);

  drawGate(window, m_andGate);
  // Draw wire labels on top of everything
  // for (const auto& wire : m_wires) {
  //     if (!wire.label.empty()) {
  //         drawWireLabel(window, wire);
  //     }
  // }
}

void DatapathView::drawComponentBox(sf::RenderWindow& window,
                                    const ComponentBox& box, sf::Color color) {
  // Draw component rectangle
  sf::RectangleShape rect(box.size);
  rect.setPosition(box.position);
  rect.setFillColor(color);
  rect.setOutlineColor(sf::Color::Black);
  rect.setOutlineThickness(1.f);
  window.draw(rect);

  // Draw label
  sf::Text label(m_font);
  label.setString(box.label);
  label.setFillColor(sf::Color::Black);

  // Center the label in the box
  if (box.inputs.size() == 0) {
    label.setCharacterSize(22);
    centerText(label);
    label.setPosition(box.getCenter());
    window.draw(label);
    return;
  }

  label.setCharacterSize(15);
  label.setPosition(
      box.getRight(3, static_cast<float>(label.getCharacterSize())));
  window.draw(label);

  size_t inputsSize = box.inputs.size();
  for (size_t i = 0; i < inputsSize; ++i) {
    sf::Text input(m_font);
    input.setString(box.inputs[i]);
    input.setFillColor(sf::Color::Black);
    input.setCharacterSize(12);
    input.setPosition(box.getLeft(i, input.getCharacterSize()));
    window.draw(input);
  }

  size_t outputsSize = box.outputs.size();
  for (size_t i = 0; i < outputsSize; ++i) {
    sf::Text output(m_font);
    output.setString(box.outputs[i]);
    output.setFillColor(sf::Color::Black);
    output.setCharacterSize(12);
    output.setPosition(
        box.getRight(i, static_cast<float>(output.getCharacterSize()), 10.f));
    window.draw(output);
  }
}

void DatapathView::drawEllipse(sf::RenderWindow& window,
                               std::unique_ptr<EllipseShape>& circle,
                               sf::Color color) {
  circle->setFillColor(color);
  circle->setOutlineColor(sf::Color::Black);
  circle->setOutlineThickness(1.f);
  window.draw(*circle);

  sf::Text label(m_font, circle->getLabel(), 12);
  label.setFillColor(sf::Color::Black);
  sf::Vector2f circlePos = circle->getPosition();
  sf::Vector2f circleRadius = circle->getRadius();
  label.setPosition(
      sf::Vector2f(circlePos.x + circleRadius.x, circlePos.y + circleRadius.y));
  centerText(label);
  window.draw(label);
}

void DatapathView::drawALU(sf::RenderWindow& window,
                           std::unique_ptr<ALUShape>& alu, sf::Color color) {
  alu->setFillColor(color);
  sf::RenderStates states;
  alu->draw(window, states);
}

void DatapathView::drawMux(sf::RenderWindow& window,
                           std::unique_ptr<MuxShape>& mux, sf::Color color) {
  mux->setFillColor(color);
  sf::RenderStates states;
  mux->draw(window, states);
}

void DatapathView::drawGate(sf::RenderWindow& window,
                            std::unique_ptr<AndGateShape>& gate,
                            sf::Color color) {
  gate->setFillColor(color);
  sf::RenderStates states;
  gate->draw(window, states);
}

void DatapathView::drawWire(sf::RenderWindow& window, const Wire& wire) {
  // Determine color based on active state
  sf::Color lineColor = wire.active ? sf::Color::Yellow : wire.color;

  // Draw the line
  sf::Vertex line[] = {sf::Vertex{{wire.start.x, wire.start.y}, lineColor},
                       sf::Vertex{{wire.end.x, wire.end.y}, lineColor}};

  // Make active wires thicker by drawing multiple lines
  if (wire.active) {
    sf::Vertex thickLine1[] = {
        sf::Vertex{{wire.start.x + 1.f, wire.start.y}, lineColor},
        sf::Vertex{{wire.end.x + 1.f, wire.end.y}, lineColor}};
    sf::Vertex thickLine2[] = {
        sf::Vertex{{wire.start.x, wire.start.y + 1.f}, lineColor},
        sf::Vertex{{wire.end.x, wire.end.y + 1.f}, lineColor}};
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
  sf::Vector2f midpoint = {(wire.start.x + wire.end.x) / 2.f,
                           (wire.start.y + wire.end.y) / 2.f};

  // Create label text
  sf::Text label(m_font);
  label.setString(wire.label);
  label.setCharacterSize(10);
  label.setFillColor(sf::Color::White);

  // Create background box for label
  sf::FloatRect textBounds = label.getLocalBounds();
  sf::RectangleShape background(
      {textBounds.size.x + 6.f, textBounds.size.y + 6.f});
  background.setPosition({midpoint.x - textBounds.size.x / 2.f - 3.f,
                          midpoint.y - textBounds.size.y / 2.f - 3.f});
  background.setFillColor(sf::Color(40, 40, 50, 220));  // Semi-transparent
  background.setOutlineColor(wire.color);
  background.setOutlineThickness(1.f);

  // Position label at midpoint
  label.setPosition({midpoint.x - textBounds.size.x / 2.f,
                     midpoint.y - textBounds.size.y / 2.f});

  window.draw(background);
  window.draw(label);
}

void DatapathView::centerText(sf::Text& label) {
  sf::FloatRect textBounds = label.getLocalBounds();
  label.setOrigin({textBounds.position.x + textBounds.size.x / 2.0f,
                   textBounds.position.y + textBounds.size.y / 2.0f});
}

}  // namespace ez_arch
