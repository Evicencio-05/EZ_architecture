#include "gui/datapath_view.hpp"
#include "gui/style.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <memory>
#include <string>

namespace ez_arch {

static void centerText(sf::Text& label) {
  sf::FloatRect textBounds = label.getLocalBounds();
  label.setOrigin({textBounds.position.x + textBounds.size.x / 2.0f,
                   textBounds.position.y + textBounds.size.y / 2.0f});
}

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

  m_wires = {};

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
  updateWirePosition(x, y);
  m_x = x;
  m_y = y;
  calculateLayout();
}

void DatapathView::setSize(float width, float height) {
  m_width = width;
  m_height = height;
  calculateLayout();
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
  m_pcBox.position = {m_x + 100.f, m_y + 485.f};
  m_pcBox.size = {50.f, 100.f};

  m_pcAlu->setPosition(sf::Vector2f(350.f + m_x, 75.f + m_y));

  m_instructionMemory.position = {m_x + 250.f, m_y + 500.f};
  m_instructionMemory.size = {150.f, 150.f};

  m_jumpSL->setRadius(sf::Vector2f(25.f, 25.f));
  m_jumpSL->setPosition(sf::Vector2f(m_x + 500.f, m_y + 50.f));

  m_regMux->setPosition(sf::Vector2f(675.f + m_x, 600.f + m_y));

  m_control->setRadius(sf::Vector2f(40.f, 125.f));
  m_control->setPosition(sf::Vector2f(800.f, 275.f));

  m_registers.position = {m_x + 750.f, m_y + 480.f};
  m_registers.size = {150.f, 250.f};

  m_signExt->setRadius(sf::Vector2f(50.f, 50.f));
  m_signExt->setPosition(sf::Vector2f(m_x + 800.f, m_y + 800.f));

  m_branchSL->setRadius(sf::Vector2f(25.f, 25.f));
  m_branchSL->setPosition(sf::Vector2f(m_x + 1000.f, m_y + 150.f));

  m_branchAlu->setPosition(sf::Vector2f(1100.f + m_x, 100.f + m_y));

  m_dataMux->setPosition(sf::Vector2f(1050.f + m_x, 600.f + m_y));

  m_dataAlu->setPosition(sf::Vector2f(1150.f + m_x, 490.f + m_y));
  m_dataAlu->setAluScale(sf::Vector2f(2.f, 2.f));

  m_ALUControl->setRadius(sf::Vector2f(50.f, 50.f));
  m_ALUControl->setPosition(sf::Vector2f(m_x + 1100.f, m_y + 800.f));

  m_branchMux->setPosition(sf::Vector2f(1350.f + m_x, 70.f + m_y));

  m_andGate->setPosition(sf::Vector2f(1300.f + m_x, 200.f + m_y));

  m_jumpMux->setPosition(sf::Vector2f(1450.f + m_x, 70.f + m_y));

  m_dataMemory.position = {m_x + 1350.f, m_y + 545.f};
  m_dataMemory.size = {150.f, 200.f};

  m_writeMux->setPosition(sf::Vector2f(1550.f + m_x, 610.f + m_y));
}

void DatapathView::updateWirePosition(float x, float y) {
  for (Wire& wire : m_wires) {
    size_t length = wire.vertices->getVertexCount();
    for (size_t i = 0; i < length; ++i) {
      sf::Vector2f currentPosition = wire.vertices->operator[](i).position;
      wire.vertices->operator[](i).position = sf::Vector2f(
          currentPosition.x - (m_x - x), currentPosition.y - (m_y - y));
    }
  }
}

void DatapathView::update() {
  // Update will be used to highlight active paths based on current instruction
}

void DatapathView::draw(sf::RenderWindow& window) {
  // Draw wires first so they appear behind components
  for (auto& wire : m_wires) {
    drawWire(window, wire);
  }

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

void DatapathView::drawWire(sf::RenderWindow& window, Wire& wire) {
  // Determine color based on active state
  sf::Color lineColor =
      wire.active ? wire.color
                  : sf::Color(wire.color.r, wire.color.g, wire.color.b, 75);

  // Draw the line
  size_t length = wire.vertices->getVertexCount();
  for (size_t i = 0; i < length; ++i) {
    wire.vertices->operator[](i).color = lineColor;
  }
  window.draw(*wire.vertices);

  // Used for identifying wires
  // if (length >= 2) {
  //   sf::Text beginning(m_font, wire.label, 15);
  //   beginning.setFillColor(sf::Color::Black);
  //   sf::Vector2f first = wire.vertices->operator[](0).position;
  //   beginning.setPosition(first);
  //   centerText(beginning);
  //   window.draw(beginning);
  //
  //   sf::Text end(m_font, wire.label, 15);
  //   end.setFillColor(sf::Color::Black);
  //   sf::Vector2f last = wire.vertices->operator[](length - 1).position;
  //   end.setPosition(last);
  //   centerText(end);
  //   window.draw(end);
  // }

  // Draw arrowhead at end
  if (!wire.connection && !wire.no_arrow && length >= 2) {
    sf::Vector2f backPositon = wire.vertices->operator[](length - 1).position;
    sf::VertexArray arrowhead(sf::PrimitiveType::Triangles, 3);
    arrowhead[0].position =
        sf::Vector2f(backPositon.x - 5.f, backPositon.y - 6.f);
    arrowhead[0].color = lineColor;
    arrowhead[1].position = backPositon;
    arrowhead[1].color = lineColor;
    arrowhead[2].position =
        sf::Vector2f(backPositon.x - 5.f, backPositon.y + 5.f);
    arrowhead[2].color = lineColor;

    window.draw(arrowhead);
  }
}

void DatapathView::setupWires() {
  // m_pcBox -> m_instructionMemory - Read Address
  m_wires[0].vertices->append(sf::Vertex{{150.f + m_x, 535.f + m_y}});
  m_wires[0].vertices->append(sf::Vertex{{250.f + m_x, 535.f + m_y}});
  m_wires[0].label = "0";

  // PC -> m_pcAlu - PC + 4
  m_wires[1].vertices->append(sf::Vertex{{175.f + m_x, 535.f + m_y}});
  m_wires[1].vertices->append(sf::Vertex{{175.f + m_x, 90.f + m_y}});
  m_wires[1].vertices->append(sf::Vertex{{350.f + m_x, 90.f + m_y}});
  m_wires[1].label = "1";

  // 4 -> m_pcAlu
  m_wires[2].vertices->append(sf::Vertex{{300.f + m_x, 150.f + m_y}});
  m_wires[2].vertices->append(sf::Vertex{{350.f + m_x, 150.f + m_y}});
  m_wires[2].label = "2";

  // I[31-0] -> m_control
  m_wires[3].vertices->append(sf::Vertex{{400.f + m_x, 560.f + m_y}});
  m_wires[3].vertices->append(sf::Vertex{{550.f + m_x, 560.f + m_y}});
  m_wires[3].vertices->append(sf::Vertex{{550.f + m_x, 340.f + m_y}});
  m_wires[3].vertices->append(sf::Vertex{{740.f + m_x, 340.f + m_y}});
  m_wires[3].label = "3";

  // I[25-21] -> m_registers - Read Reg 1
  m_wires[4].vertices->append(sf::Vertex{{550.f + m_x, 510.f + m_y}});
  m_wires[4].vertices->append(sf::Vertex{{750.f + m_x, 510.f + m_y}});
  m_wires[4].label = "4";

  // I[15-0] -> m_signExt
  m_wires[5].vertices->append(sf::Vertex{{550.f + m_x, 560.f + m_y}});
  m_wires[5].vertices->append(sf::Vertex{{550.f + m_x, 850.f + m_y}});
  m_wires[5].vertices->append(sf::Vertex{{800.f + m_x, 850.f + m_y}});
  m_wires[5].label = "5";

  // I[20-16] -> m_registers - Read Reg 2
  m_wires[6].vertices->append(sf::Vertex{{550.f + m_x, 575.f + m_y}});
  m_wires[6].vertices->append(sf::Vertex{{750.f + m_x, 575.f + m_y}});
  m_wires[6].label = "6";

  // I[20-16] -> m_regMux - 0
  m_wires[7].vertices->append(sf::Vertex{{625.f + m_x, 575.f + m_y}});
  m_wires[7].vertices->append(sf::Vertex{{625.f + m_x, 612.f + m_y}});
  m_wires[7].vertices->append(sf::Vertex{{675.f + m_x, 612.f + m_y}});
  m_wires[7].label = "7";

  // I[15-11] -> m_regMux - 1
  m_wires[8].vertices->append(sf::Vertex{{550.f + m_x, 677.f + m_y}});
  m_wires[8].vertices->append(sf::Vertex{{675.f + m_x, 677.f + m_y}});
  m_wires[8].label = "8";

  // regMux -> m_registers - Write Reg
  m_wires[9].vertices->append(sf::Vertex{{700.f + m_x, 640.f + m_y}});
  m_wires[9].vertices->append(sf::Vertex{{750.f + m_x, 640.f + m_y}});
  m_wires[9].label = "9";

  // I[31-26] -> m_jumpSL
  m_wires[10].vertices->append(sf::Vertex{{425.f + m_x, 560.f + m_y}});
  m_wires[10].vertices->append(sf::Vertex{{425.f + m_x, 75.f + m_y}});
  m_wires[10].vertices->append(sf::Vertex{{500.f + m_x, 75.f + m_y}});
  m_wires[10].label = "10";

  // JA[31-0] -> m_jumpMux - 1
  m_wires[11].vertices->append(sf::Vertex{{550.f + m_x, 75.f + m_y}});
  m_wires[11].vertices->append(sf::Vertex{{1000.f + m_x, 75.f + m_y}});
  m_wires[11].vertices->append(sf::Vertex{{1000.f + m_x, 50.f + m_y}});
  m_wires[11].vertices->append(sf::Vertex{{1420.f + m_x, 50.f + m_y}});
  m_wires[11].vertices->append(sf::Vertex{{1420.f + m_x, 85.f + m_y}});
  m_wires[11].vertices->append(sf::Vertex{{1450.f + m_x, 85.f + m_y}});
  m_wires[11].label = "11";

  // I[5-0] -> m_ALUControl
  m_wires[12].vertices->append(sf::Vertex{{700.f + m_x, 850.f + m_y}});
  m_wires[12].vertices->append(sf::Vertex{{700.f + m_x, 925.f + m_y}});
  m_wires[12].vertices->append(sf::Vertex{{1050.f + m_x, 925.f + m_y}});
  m_wires[12].vertices->append(sf::Vertex{{1050.f + m_x, 850.f + m_y}});
  m_wires[12].vertices->append(sf::Vertex{{1100.f + m_x, 850.f + m_y}});
  m_wires[12].label = "12";

  // m_registers - Read Data 1 -> m_dataAlu - Top
  m_wires[13].vertices->append(sf::Vertex{{900.f + m_x, 520.f + m_y}});
  m_wires[13].vertices->append(sf::Vertex{{1150.f + m_x, 520.f + m_y}});
  m_wires[13].label = "13";

  // m_registers - Read Data 2 -> m_dataMux - 0
  m_wires[14].vertices->append(sf::Vertex{{900.f + m_x, 640.f + m_y}});
  m_wires[14].vertices->append(sf::Vertex{{1000.f + m_x, 640.f + m_y}});
  m_wires[14].vertices->append(sf::Vertex{{1000.f + m_x, 612.f + m_y}});
  m_wires[14].vertices->append(sf::Vertex{{1050.f + m_x, 612.f + m_y}});
  m_wires[14].label = "14";

  // m_registers - Read Data 2 -> m_dataMemory - Write Data
  m_wires[15].vertices->append(sf::Vertex{{1000.f + m_x, 640.f + m_y}});
  m_wires[15].vertices->append(sf::Vertex{{1000.f + m_x, 725.f + m_y}});
  m_wires[15].vertices->append(sf::Vertex{{1350.f + m_x, 725.f + m_y}});
  m_wires[15].label = "15";

  // m_dataMux -> m_dataAlu - Bottom
  m_wires[16].vertices->append(sf::Vertex{{1075.f + m_x, 650.f + m_y}});
  m_wires[16].vertices->append(sf::Vertex{{1150.f + m_x, 650.f + m_y}});
  m_wires[16].label = "16";

  // m_signExt -> m_branchSL
  m_wires[17].vertices->append(sf::Vertex{{900.f + m_x, 850.f + m_y}});
  m_wires[17].vertices->append(sf::Vertex{{950.f + m_x, 850.f + m_y}});
  m_wires[17].vertices->append(sf::Vertex{{950.f + m_x, 175.f + m_y}});
  m_wires[17].vertices->append(sf::Vertex{{1000.f + m_x, 175.f + m_y}});
  m_wires[17].label = "17";

  // m_branchSL -> m_branchAlu - Bottom
  m_wires[18].vertices->append(sf::Vertex{{1050.f + m_x, 175.f + m_y}});
  m_wires[18].vertices->append(sf::Vertex{{1100.f + m_x, 175.f + m_y}});
  m_wires[18].label = "18";

  // m_pcAlu -> m_branchAlu - Top
  m_wires[19].vertices->append(sf::Vertex{{400.f + m_x, 125.f + m_y}});
  m_wires[19].vertices->append(sf::Vertex{{1100.f + m_x, 125.f + m_y}});
  m_wires[19].label = "19";

  // PC + 4[31-28] -> JA[31-0]
  m_wires[20].vertices->append(sf::Vertex{{700.f + m_x, 125.f + m_y}});
  m_wires[20].vertices->append(sf::Vertex{{700.f + m_x, 75.f + m_y}});
  m_wires[20].label = "20";
  m_wires[20].no_arrow = true;

  // PC + 4 -> m_branchMux - 0
  m_wires[21].vertices->append(sf::Vertex{{1050.f + m_x, 125.f + m_y}});
  m_wires[21].vertices->append(sf::Vertex{{1050.f + m_x, 80.f + m_y}});
  m_wires[21].vertices->append(sf::Vertex{{1350.f + m_x, 80.f + m_y}});
  m_wires[21].label = "21";

  // m_branchAlu -> m_branchMux - 1
  m_wires[22].vertices->append(sf::Vertex{{1150.f + m_x, 150.f + m_y}});
  m_wires[22].vertices->append(sf::Vertex{{1350.f + m_x, 150.f + m_y}});
  m_wires[22].label = "22";

  // m_branchMux -> m_jumpMux - 0
  m_wires[23].vertices->append(sf::Vertex{{1375.f + m_x, 120.f + m_y}});
  m_wires[23].vertices->append(sf::Vertex{{1420.f + m_x, 120.f + m_y}});
  m_wires[23].vertices->append(sf::Vertex{{1420.f + m_x, 150.f + m_y}});
  m_wires[23].vertices->append(sf::Vertex{{1450.f + m_x, 150.f + m_y}});
  m_wires[23].label = "23";

  // m_dataAlu - Zero -> m_andGate - Bottom
  m_wires[24].vertices->append(sf::Vertex{{1250.f + m_x, 560.f + m_y}});
  m_wires[24].vertices->append(sf::Vertex{{1275.f + m_x, 560.f + m_y}});
  m_wires[24].vertices->append(sf::Vertex{{1275.f + m_x, 235.f + m_y}});
  m_wires[24].vertices->append(sf::Vertex{{1300.f + m_x, 235.f + m_y}});
  m_wires[24].label = "24";

  // m_dataAlu - Result -> m_dataMemory - Address
  m_wires[25].vertices->append(sf::Vertex{{1250.f + m_x, 615.f + m_y}});
  m_wires[25].vertices->append(sf::Vertex{{1350.f + m_x, 615.f + m_y}});
  m_wires[25].label = "25";

  // m_dataAlu - Result -> m_writeMux - -1
  m_wires[26].vertices->append(sf::Vertex{{1320.f + m_x, 615.f + m_y}});
  m_wires[26].vertices->append(sf::Vertex{{1320.f + m_x, 805.f + m_y}});
  m_wires[26].vertices->append(sf::Vertex{{1530.f + m_x, 805.f + m_y}});
  m_wires[26].vertices->append(sf::Vertex{{1530.f + m_x, 690.f + m_y}});
  m_wires[26].vertices->append(sf::Vertex{{1550.f + m_x, 690.f + m_y}});
  m_wires[26].label = "26";

  // m_dataMemory - Read Data -> m_writeMux - 0
  m_wires[27].vertices->append(sf::Vertex{{1500.f + m_x, 625.f + m_y}});
  m_wires[27].vertices->append(sf::Vertex{{1550.f + m_x, 625.f + m_y}});
  m_wires[27].label = "27";

  // m_writeMux -> m_registers - Write Data
  m_wires[28].vertices->append(sf::Vertex{{1575.f + m_x, 620.f + m_y}});
  m_wires[28].vertices->append(sf::Vertex{{1600.f + m_x, 620.f + m_y}});
  m_wires[28].vertices->append(sf::Vertex{{1600.f + m_x, 785.f + m_y}});
  m_wires[28].vertices->append(sf::Vertex{{725.f + m_x, 785.f + m_y}});
  m_wires[28].vertices->append(sf::Vertex{{725.f + m_x, 700.f + m_y}});
  m_wires[28].vertices->append(sf::Vertex{{750.f + m_x, 700.f + m_y}});
  m_wires[28].label = "28";

  // m_jumpMux -> m_pcBox
  m_wires[29].vertices->append(sf::Vertex{{1475.f + m_x, 115.f + m_y}});
  m_wires[29].vertices->append(sf::Vertex{{1500.f + m_x, 115.f + m_y}});
  m_wires[29].vertices->append(sf::Vertex{{1500.f + m_x, 20.f + m_y}});
  m_wires[29].vertices->append(sf::Vertex{{50.f + m_x, 20.f + m_y}});
  m_wires[29].vertices->append(sf::Vertex{{50.f + m_x, 535.f + m_y}});
  m_wires[29].vertices->append(sf::Vertex{{100.f + m_x, 535.f + m_y}});
  m_wires[29].label = "29";

  // m_signExt -> m_dataMux - 0
  m_wires[30].vertices->append(sf::Vertex{{950.f + m_x, 680.f + m_y}});
  m_wires[30].vertices->append(sf::Vertex{{1050.f + m_x, 680.f + m_y}});
  m_wires[30].label = "30";

  // RegDst -> m_regMux - Control
  m_wires[31].vertices->append(sf::Vertex{{800.f + m_x, 240.f + m_y}});
  m_wires[31].vertices->append(sf::Vertex{{900.f + m_x, 240.f + m_y}});
  m_wires[31].vertices->append(sf::Vertex{{900.f + m_x, 190.f + m_y}});
  m_wires[31].vertices->append(sf::Vertex{{500.f + m_x, 190.f + m_y}});
  m_wires[31].vertices->append(sf::Vertex{{500.f + m_x, 710.f + m_y}});
  m_wires[31].vertices->append(sf::Vertex{{690.f + m_x, 710.f + m_y}});
  m_wires[31].vertices->append(sf::Vertex{{690.f + m_x, 685.f + m_y}});
  m_wires[31].label = "31";
  m_wires[31].no_arrow = true;
  m_wires[31].color = CONTROL_WIRE_COLOR;

  // Jump -> m_jumpMux - Control
  m_wires[32].vertices->append(sf::Vertex{{800.f + m_x, 265.f + m_y}});
  m_wires[32].vertices->append(sf::Vertex{{925.f + m_x, 265.f + m_y}});
  m_wires[32].vertices->append(sf::Vertex{{925.f + m_x, 35.f + m_y}});
  m_wires[32].vertices->append(sf::Vertex{{1465.f + m_x, 35.f + m_y}});
  m_wires[32].vertices->append(sf::Vertex{{1465.f + m_x, 80.f + m_y}});
  m_wires[32].label = "32";
  m_wires[32].no_arrow = true;
  m_wires[32].color = CONTROL_WIRE_COLOR;

  // Branch -> m_andGate - Top
  m_wires[33].vertices->append(sf::Vertex{{810.f + m_x, 290.f + m_y}});
  m_wires[33].vertices->append(sf::Vertex{{1200.f + m_x, 290.f + m_y}});
  m_wires[33].vertices->append(sf::Vertex{{1200.f + m_x, 215.f + m_y}});
  m_wires[33].vertices->append(sf::Vertex{{1300.f + m_x, 215.f + m_y}});
  m_wires[33].label = "33";
  m_wires[33].no_arrow = true;
  m_wires[33].color = CONTROL_WIRE_COLOR;

  // m_andGate -> m_branchMux - Control
  m_wires[34].vertices->append(sf::Vertex{{1350.f + m_x, 225.f + m_y}});
  m_wires[34].vertices->append(sf::Vertex{{1365.f + m_x, 225.f + m_y}});
  m_wires[34].vertices->append(sf::Vertex{{1365.f + m_x, 160.f + m_y}});
  m_wires[34].label = "34";
  m_wires[34].no_arrow = true;
  m_wires[34].color = CONTROL_WIRE_COLOR;

  // MemRead -> m_dataMemory - Bottom Control
  m_wires[35].vertices->append(sf::Vertex{{820.f + m_x, 315.f + m_y}});
  m_wires[35].vertices->append(sf::Vertex{{1625.f + m_x, 315.f + m_y}});
  m_wires[35].vertices->append(sf::Vertex{{1625.f + m_x, 770.f + m_y}});
  m_wires[35].vertices->append(sf::Vertex{{1425.f + m_x, 770.f + m_y}});
  m_wires[35].vertices->append(sf::Vertex{{1425.f + m_x, 740.f + m_y}});
  m_wires[35].label = "35";
  m_wires[35].no_arrow = true;
  m_wires[35].color = CONTROL_WIRE_COLOR;

  // MemtoReg -> m_writeMux - Control
  m_wires[36].vertices->append(sf::Vertex{{820.f + m_x, 340.f + m_y}});
  m_wires[36].vertices->append(sf::Vertex{{1565.f + m_x, 340.f + m_y}});
  m_wires[36].vertices->append(sf::Vertex{{1565.f + m_x, 610.f + m_y}});
  m_wires[36].label = "36";
  m_wires[36].no_arrow = true;
  m_wires[36].color = CONTROL_WIRE_COLOR;

  // ALUOp -> m_ALUControl - Control
  m_wires[37].vertices->append(sf::Vertex{{810.f + m_x, 365.f + m_y}});
  m_wires[37].vertices->append(sf::Vertex{{975.f + m_x, 365.f + m_y}});
  m_wires[37].vertices->append(sf::Vertex{{975.f + m_x, 910.f + m_y}});
  m_wires[37].vertices->append(sf::Vertex{{1150.f + m_x, 910.f + m_y}});
  m_wires[37].vertices->append(sf::Vertex{{1150.f + m_x, 900.f + m_y}});
  m_wires[37].label = "37";
  m_wires[37].no_arrow = true;
  m_wires[37].color = CONTROL_WIRE_COLOR;

  // m_ALUControl -> m_dataAlu - Control
  m_wires[38].vertices->append(sf::Vertex{{1200.f + m_x, 850.f + m_y}});
  m_wires[38].vertices->append(sf::Vertex{{1225.f + m_x, 850.f + m_y}});
  m_wires[38].vertices->append(sf::Vertex{{1225.f + m_x, 650.f + m_y}});
  m_wires[38].label = "38";
  m_wires[38].no_arrow = true;
  m_wires[38].color = CONTROL_WIRE_COLOR;

  // MemWrite -> m_dataMemory - Top Control
  m_wires[39].vertices->append(sf::Vertex{{810.f + m_x, 390.f + m_y}});
  m_wires[39].vertices->append(sf::Vertex{{1425.f + m_x, 390.f + m_y}});
  m_wires[39].vertices->append(sf::Vertex{{1425.f + m_x, 550.f + m_y}});
  m_wires[39].label = "39";
  m_wires[39].no_arrow = true;
  m_wires[39].color = CONTROL_WIRE_COLOR;

  // ALUSrc -> m_dataMux - Control
  m_wires[40].vertices->append(sf::Vertex{{800.f + m_x, 415.f + m_y}});
  m_wires[40].vertices->append(sf::Vertex{{1065.f + m_x, 415.f + m_y}});
  m_wires[40].vertices->append(sf::Vertex{{1065.f + m_x, 600.f + m_y}});
  m_wires[40].label = "40";
  m_wires[40].no_arrow = true;
  m_wires[40].color = CONTROL_WIRE_COLOR;

  // RegWrite -> m_registers - Control
  m_wires[41].vertices->append(sf::Vertex{{800.f + m_x, 440.f + m_y}});
  m_wires[41].vertices->append(sf::Vertex{{885.f + m_x, 440.f + m_y}});
  m_wires[41].vertices->append(sf::Vertex{{885.f + m_x, 500.f + m_y}});
  m_wires[41].label = "41";
  m_wires[41].no_arrow = true;
  m_wires[41].color = CONTROL_WIRE_COLOR;

}

}  // namespace ez_arch
