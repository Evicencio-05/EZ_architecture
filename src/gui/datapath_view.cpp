#include "gui/datapath_view.hpp"

#include "core/decoder.hpp"
#include "gui/style.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Window.hpp>
#include <cstddef>
#include <iostream>
#include <memory>
#include <string>


namespace ez_arch {

// TODO(evice): Update to draw and center
static void centerText(sf::Text& label) {
  sf::FloatRect textBounds = label.getLocalBounds();
  label.setOrigin({textBounds.position.x + (textBounds.size.x / 2.0F),
                   textBounds.position.y + (textBounds.size.y / 2.0F)});
}

DatapathView::DatapathView(const CPU& cpu, sf::Font& font)
    : m_cpu(cpu), m_font(font) {
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
  setupWireLabels();

  m_pcBox.label = "PC";

  m_instructionMemory.label = "Instruction\n Memory";
  m_instructionMemory.inputs.emplace_back("Read\nAddress");
  m_instructionMemory.inputs.emplace_back("");
  m_instructionMemory.outputs[1] = "Instructions\n   [31-0]";

  m_registers.label = "\nRegisters";
  m_registers.inputs.emplace_back("Read\nRegister 1");
  m_registers.inputs.emplace_back("Read\nRegister 2");
  m_registers.inputs.emplace_back("Write\nRegister");
  m_registers.inputs.emplace_back("Write\nData");
  m_registers.outputs[0] = "        Read\n        Data 1";
  m_registers.outputs[2] = "        Read\n        Data 2";

  m_dataMemory.label = "     Data\n   Memory";
  m_dataMemory.inputs.emplace_back("");
  m_dataMemory.inputs.emplace_back("Address");
  m_dataMemory.inputs.emplace_back("");
  m_dataMemory.inputs.emplace_back("Write\nData");
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
  updateWireLabelPosition(x, y);
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
      sf::Vertex point {
          {static_cast<float>(i) + m_x, static_cast<float>(j) + m_y},
          sf::Color::Black};
      window.draw(&point, 1, sf::PrimitiveType::Points);
    }
  }
}

void DatapathView::calculateLayout() {
  // Layout the datapath components in a logical flow (left to right)
  m_pcBox.position = {m_x + 100.F, m_y + 485.F};
  m_pcBox.size = {50.F, 100.F};

  m_pcAlu->setPosition(sf::Vector2f(350.F + m_x, 75.F + m_y));

  m_instructionMemory.position = {m_x + 250.F, m_y + 500.F};
  m_instructionMemory.size = {150.F, 150.F};

  m_jumpSL->setRadius(sf::Vector2f(25.F, 25.F));
  m_jumpSL->setPosition(sf::Vector2f(m_x + 500.F, m_y + 50.F));

  m_regMux->setPosition(sf::Vector2f(675.F + m_x, 600.F + m_y));

  m_control->setRadius(sf::Vector2f(40.F, 125.F));
  m_control->setPosition(sf::Vector2f(800.F, 275.F));

  m_registers.position = {m_x + 750.F, m_y + 480.F};
  m_registers.size = {150.F, 250.F};

  m_signExt->setRadius(sf::Vector2f(50.F, 50.F));
  m_signExt->setPosition(sf::Vector2f(m_x + 800.F, m_y + 800.F));

  m_branchSL->setRadius(sf::Vector2f(25.F, 25.F));
  m_branchSL->setPosition(sf::Vector2f(m_x + 1000.F, m_y + 150.F));

  m_branchAlu->setPosition(sf::Vector2f(1100.F + m_x, 100.F + m_y));

  m_dataMux->setPosition(sf::Vector2f(1050.F + m_x, 600.F + m_y));

  m_dataAlu->setPosition(sf::Vector2f(1150.F + m_x, 490.F + m_y));
  m_dataAlu->setAluScale(sf::Vector2f(2.F, 2.F));

  m_ALUControl->setRadius(sf::Vector2f(50.F, 50.F));
  m_ALUControl->setPosition(sf::Vector2f(m_x + 1100.F, m_y + 800.F));

  m_branchMux->setPosition(sf::Vector2f(1350.F + m_x, 70.F + m_y));

  m_andGate->setPosition(sf::Vector2f(1300.F + m_x, 200.F + m_y));

  m_jumpMux->setPosition(sf::Vector2f(1450.F + m_x, 70.F + m_y));

  m_dataMemory.position = {m_x + 1350.F, m_y + 545.F};
  m_dataMemory.size = {150.F, 200.F};

  m_writeMux->setPosition(sf::Vector2f(1550.F + m_x, 610.F + m_y));
}

void DatapathView::updateWirePosition(float x, float y) {
  for (Wire& wire : m_wires) {
    size_t length = wire.vertices->getVertexCount();
    for (size_t i = 0; i < length; ++i) {
      sf::Vector2f currentPosition = wire.vertices->operator[](i).position;
      wire.vertices->operator[](i).position = sf::Vector2f(
          currentPosition.x - m_x + x, currentPosition.y - m_y + y);
    }
  }
}

void DatapathView::updateWireLabelPosition(float x, float y) {
  for (size_t i = 0; i < DatapathView::kNUMBER_OF_WIRE_LABLES; ++i) {
    sf::Vector2f currentPositon = m_wireLabels[i].position;
    m_wireLabels[i].position = {currentPositon.x - m_x + x,
                                currentPositon.y - m_y + y};
  }
}

void DatapathView::update() {
  word_t rawInstruction = m_cpu.getCurrentInstruction().getRaw();
  Decoder decoder;
  std::string currentInstructionName =
      ez_arch::Decoder::getDetails(rawInstruction).mnemonic;

  for (auto& wire : m_wires) {
    wire.active = true;
  }

  std::vector<size_t> offWires;
  offWires.reserve(m_wires.size());


  // Update will be used to highlight active paths based on current instruction
}

void DatapathView::draw(sf::RenderWindow& window) {
  // Draw wires first so they appear behind components
  for (auto& wire : m_wires) {
    drawWire(window, wire);
  }

  for (size_t i = 0; i < DatapathView::kNUMBER_OF_WIRE_LABLES; ++i) {
    drawLabel(window, m_wireLabels[i]);
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

  drawAlu(window, m_pcAlu);
  drawAlu(window, m_branchAlu);
  drawAlu(window, m_dataAlu);

  drawMux(window, m_regMux);
  drawMux(window, m_dataMux);
  drawMux(window, m_branchMux);
  drawMux(window, m_jumpMux);
  drawMux(window, m_writeMux);

  drawGate(window, m_andGate);
}

void DatapathView::drawComponentBox(sf::RenderWindow& window,
                                    const ComponentBox& box,
                                    sf::Color color) {
  // Draw component rectangle
  sf::RectangleShape rect(box.size);
  rect.setPosition(box.position);
  rect.setFillColor(color);
  rect.setOutlineColor(sf::Color::Black);
  rect.setOutlineThickness(1.F);
  window.draw(rect);

  // Draw label
  sf::Text label(m_font);
  label.setString(box.label);
  label.setFillColor(sf::Color::Black);

  // Center the label in the box
  if (box.inputs.empty()) {
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
        box.getRight(i, static_cast<float>(output.getCharacterSize()), 10.F));
    window.draw(output);
  }
}

void DatapathView::drawEllipse(sf::RenderWindow& window,
                               std::unique_ptr<EllipseShape>& circle,
                               sf::Color color) {
  circle->setFillColor(color);
  circle->setOutlineColor(sf::Color::Black);
  circle->setOutlineThickness(1.F);
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

void DatapathView::drawAlu(sf::RenderWindow& window,
                           std::unique_ptr<ALUShape>& alu,
                           sf::Color color) {
  alu->setFillColor(color);
  sf::RenderStates states;
  alu->draw(window, states);
}

void DatapathView::drawMux(sf::RenderWindow& window,
                           std::unique_ptr<MuxShape>& mux,
                           sf::Color color) {
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
  //   sf::Text beginning(m_font, wire.number, 15);
  //   beginning.setFillColor(sf::Color::Black);
  //   sf::Vector2f first = wire.vertices->operator[](0).position;
  //   beginning.setPosition(first);
  //   centerText(beginning);
  //   window.draw(beginning);
  //
  //   sf::Text end(m_font, wire.number, 15);
  //   end.setFillColor(sf::Color::Black);
  //   sf::Vector2f last = wire.vertices->operator[](length - 1).position;
  //   end.setPosition(last);
  //   centerText(end);
  //   window.draw(end);
  // }

  // Draw arrowhead at end
  if (!wire.no_arrow && length >= 2) {
    sf::Vector2f backPositon = wire.vertices->operator[](length - 1).position;
    sf::VertexArray arrowhead(sf::PrimitiveType::Triangles, 3);
    arrowhead[0].position =
        sf::Vector2f(backPositon.x - 5.F, backPositon.y - 6.F);
    arrowhead[0].color = lineColor;
    arrowhead[1].position = backPositon;
    arrowhead[1].color = lineColor;
    arrowhead[2].position =
        sf::Vector2f(backPositon.x - 5.F, backPositon.y + 5.F);
    arrowhead[2].color = lineColor;

    window.draw(arrowhead);
  }
}

void DatapathView::setupWires() {
  for (Wire& wire : m_wires) {
    wire.vertices->clear();
  }
  // m_pcBox -> m_instructionMemory - Read Address
  m_wires[0].vertices->append(sf::Vertex {{150.F + m_x, 535.F + m_y}});
  m_wires[0].vertices->append(sf::Vertex {{250.F + m_x, 535.F + m_y}});
  m_wires[0].number = "0";

  // PC -> m_pcAlu - PC + 4
  m_wires[1].vertices->append(sf::Vertex {{175.F + m_x, 535.F + m_y}});
  m_wires[1].vertices->append(sf::Vertex {{175.F + m_x, 90.F + m_y}});
  m_wires[1].vertices->append(sf::Vertex {{350.F + m_x, 90.F + m_y}});
  m_wires[1].number = "1";

  // 4 -> m_pcAlu
  m_wires[2].vertices->append(sf::Vertex {{300.F + m_x, 150.F + m_y}});
  m_wires[2].vertices->append(sf::Vertex {{350.F + m_x, 150.F + m_y}});
  m_wires[2].number = "2";

  // I[31-0] -> m_control
  m_wires[3].vertices->append(sf::Vertex {{400.F + m_x, 560.F + m_y}});
  m_wires[3].vertices->append(sf::Vertex {{550.F + m_x, 560.F + m_y}});
  m_wires[3].vertices->append(sf::Vertex {{550.F + m_x, 340.F + m_y}});
  m_wires[3].vertices->append(sf::Vertex {{740.F + m_x, 340.F + m_y}});
  m_wires[3].number = "3";

  // I[25-21] -> m_registers - Read Reg 1
  m_wires[4].vertices->append(sf::Vertex {{550.F + m_x, 510.F + m_y}});
  m_wires[4].vertices->append(sf::Vertex {{750.F + m_x, 510.F + m_y}});
  m_wires[4].number = "4";

  // I[15-0] -> m_signExt
  m_wires[5].vertices->append(sf::Vertex {{550.F + m_x, 560.F + m_y}});
  m_wires[5].vertices->append(sf::Vertex {{550.F + m_x, 850.F + m_y}});
  m_wires[5].vertices->append(sf::Vertex {{800.F + m_x, 850.F + m_y}});
  m_wires[5].number = "5";

  // I[20-16] -> m_registers - Read Reg 2
  m_wires[6].vertices->append(sf::Vertex {{550.F + m_x, 575.F + m_y}});
  m_wires[6].vertices->append(sf::Vertex {{750.F + m_x, 575.F + m_y}});
  m_wires[6].number = "6";

  // I[20-16] -> m_regMux - 0
  m_wires[7].vertices->append(sf::Vertex {{625.F + m_x, 575.F + m_y}});
  m_wires[7].vertices->append(sf::Vertex {{625.F + m_x, 612.F + m_y}});
  m_wires[7].vertices->append(sf::Vertex {{675.F + m_x, 612.F + m_y}});
  m_wires[7].number = "7";

  // I[15-11] -> m_regMux - 1
  m_wires[8].vertices->append(sf::Vertex {{550.F + m_x, 677.F + m_y}});
  m_wires[8].vertices->append(sf::Vertex {{675.F + m_x, 677.F + m_y}});
  m_wires[8].number = "8";

  // regMux -> m_registers - Write Reg
  m_wires[9].vertices->append(sf::Vertex {{700.F + m_x, 640.F + m_y}});
  m_wires[9].vertices->append(sf::Vertex {{750.F + m_x, 640.F + m_y}});
  m_wires[9].number = "9";

  // I[31-26] -> m_jumpSL
  m_wires[10].vertices->append(sf::Vertex {{425.F + m_x, 560.F + m_y}});
  m_wires[10].vertices->append(sf::Vertex {{425.F + m_x, 75.F + m_y}});
  m_wires[10].vertices->append(sf::Vertex {{500.F + m_x, 75.F + m_y}});
  m_wires[10].number = "10";

  // JA[31-0] -> m_jumpMux - 1
  m_wires[11].vertices->append(sf::Vertex {{550.F + m_x, 75.F + m_y}});
  m_wires[11].vertices->append(sf::Vertex {{1000.F + m_x, 75.F + m_y}});
  m_wires[11].vertices->append(sf::Vertex {{1000.F + m_x, 50.F + m_y}});
  m_wires[11].vertices->append(sf::Vertex {{1420.F + m_x, 50.F + m_y}});
  m_wires[11].vertices->append(sf::Vertex {{1420.F + m_x, 85.F + m_y}});
  m_wires[11].vertices->append(sf::Vertex {{1450.F + m_x, 85.F + m_y}});
  m_wires[11].number = "11";

  // I[5-0] -> m_ALUControl
  m_wires[12].vertices->append(sf::Vertex {{700.F + m_x, 850.F + m_y}});
  m_wires[12].vertices->append(sf::Vertex {{700.F + m_x, 925.F + m_y}});
  m_wires[12].vertices->append(sf::Vertex {{1050.F + m_x, 925.F + m_y}});
  m_wires[12].vertices->append(sf::Vertex {{1050.F + m_x, 850.F + m_y}});
  m_wires[12].vertices->append(sf::Vertex {{1100.F + m_x, 850.F + m_y}});
  m_wires[12].number = "12";

  // m_registers - Read Data 1 -> m_dataAlu - Top
  m_wires[13].vertices->append(sf::Vertex {{900.F + m_x, 520.F + m_y}});
  m_wires[13].vertices->append(sf::Vertex {{1150.F + m_x, 520.F + m_y}});
  m_wires[13].number = "13";

  // m_registers - Read Data 2 -> m_dataMux - 0
  m_wires[14].vertices->append(sf::Vertex {{900.F + m_x, 640.F + m_y}});
  m_wires[14].vertices->append(sf::Vertex {{1000.F + m_x, 640.F + m_y}});
  m_wires[14].vertices->append(sf::Vertex {{1000.F + m_x, 612.F + m_y}});
  m_wires[14].vertices->append(sf::Vertex {{1050.F + m_x, 612.F + m_y}});
  m_wires[14].number = "14";

  // m_registers - Read Data 2 -> m_dataMemory - Write Data
  m_wires[15].vertices->append(sf::Vertex {{1000.F + m_x, 640.F + m_y}});
  m_wires[15].vertices->append(sf::Vertex {{1000.F + m_x, 725.F + m_y}});
  m_wires[15].vertices->append(sf::Vertex {{1350.F + m_x, 725.F + m_y}});
  m_wires[15].number = "15";

  // m_dataMux -> m_dataAlu - Bottom
  m_wires[16].vertices->append(sf::Vertex {{1075.F + m_x, 650.F + m_y}});
  m_wires[16].vertices->append(sf::Vertex {{1150.F + m_x, 650.F + m_y}});
  m_wires[16].number = "16";

  // m_signExt -> m_branchSL
  m_wires[17].vertices->append(sf::Vertex {{900.F + m_x, 850.F + m_y}});
  m_wires[17].vertices->append(sf::Vertex {{950.F + m_x, 850.F + m_y}});
  m_wires[17].vertices->append(sf::Vertex {{950.F + m_x, 175.F + m_y}});
  m_wires[17].vertices->append(sf::Vertex {{1000.F + m_x, 175.F + m_y}});
  m_wires[17].number = "17";

  // m_branchSL -> m_branchAlu - Bottom
  m_wires[18].vertices->append(sf::Vertex {{1050.F + m_x, 175.F + m_y}});
  m_wires[18].vertices->append(sf::Vertex {{1100.F + m_x, 175.F + m_y}});
  m_wires[18].number = "18";

  // m_pcAlu -> m_branchAlu - Top
  m_wires[19].vertices->append(sf::Vertex {{400.F + m_x, 125.F + m_y}});
  m_wires[19].vertices->append(sf::Vertex {{1100.F + m_x, 125.F + m_y}});
  m_wires[19].number = "19";

  // PC + 4[31-28] -> JA[31-0]
  m_wires[20].vertices->append(sf::Vertex {{700.F + m_x, 125.F + m_y}});
  m_wires[20].vertices->append(sf::Vertex {{700.F + m_x, 75.F + m_y}});
  m_wires[20].number = "20";
  m_wires[20].no_arrow = true;

  // PC + 4 -> m_branchMux - 0
  m_wires[21].vertices->append(sf::Vertex {{1050.F + m_x, 125.F + m_y}});
  m_wires[21].vertices->append(sf::Vertex {{1050.F + m_x, 80.F + m_y}});
  m_wires[21].vertices->append(sf::Vertex {{1350.F + m_x, 80.F + m_y}});
  m_wires[21].number = "21";

  // m_branchAlu -> m_branchMux - 1
  m_wires[22].vertices->append(sf::Vertex {{1150.F + m_x, 150.F + m_y}});
  m_wires[22].vertices->append(sf::Vertex {{1350.F + m_x, 150.F + m_y}});
  m_wires[22].number = "22";

  // m_branchMux -> m_jumpMux - 0
  m_wires[23].vertices->append(sf::Vertex {{1375.F + m_x, 120.F + m_y}});
  m_wires[23].vertices->append(sf::Vertex {{1420.F + m_x, 120.F + m_y}});
  m_wires[23].vertices->append(sf::Vertex {{1420.F + m_x, 150.F + m_y}});
  m_wires[23].vertices->append(sf::Vertex {{1450.F + m_x, 150.F + m_y}});
  m_wires[23].number = "23";

  // m_dataAlu - Zero -> m_andGate - Bottom
  m_wires[24].vertices->append(sf::Vertex {{1250.F + m_x, 560.F + m_y}});
  m_wires[24].vertices->append(sf::Vertex {{1275.F + m_x, 560.F + m_y}});
  m_wires[24].vertices->append(sf::Vertex {{1275.F + m_x, 235.F + m_y}});
  m_wires[24].vertices->append(sf::Vertex {{1300.F + m_x, 235.F + m_y}});
  m_wires[24].number = "24";

  // m_dataAlu - Result -> m_dataMemory - Address
  m_wires[25].vertices->append(sf::Vertex {{1250.F + m_x, 615.F + m_y}});
  m_wires[25].vertices->append(sf::Vertex {{1350.F + m_x, 615.F + m_y}});
  m_wires[25].number = "25";

  // m_dataAlu - Result -> m_writeMux - -1
  m_wires[26].vertices->append(sf::Vertex {{1320.F + m_x, 615.F + m_y}});
  m_wires[26].vertices->append(sf::Vertex {{1320.F + m_x, 805.F + m_y}});
  m_wires[26].vertices->append(sf::Vertex {{1530.F + m_x, 805.F + m_y}});
  m_wires[26].vertices->append(sf::Vertex {{1530.F + m_x, 690.F + m_y}});
  m_wires[26].vertices->append(sf::Vertex {{1550.F + m_x, 690.F + m_y}});
  m_wires[26].number = "26";

  // m_dataMemory - Read Data -> m_writeMux - 0
  m_wires[27].vertices->append(sf::Vertex {{1500.F + m_x, 625.F + m_y}});
  m_wires[27].vertices->append(sf::Vertex {{1550.F + m_x, 625.F + m_y}});
  m_wires[27].number = "27";

  // m_writeMux -> m_registers - Write Data
  m_wires[28].vertices->append(sf::Vertex {{1575.F + m_x, 620.F + m_y}});
  m_wires[28].vertices->append(sf::Vertex {{1600.F + m_x, 620.F + m_y}});
  m_wires[28].vertices->append(sf::Vertex {{1600.F + m_x, 785.F + m_y}});
  m_wires[28].vertices->append(sf::Vertex {{725.F + m_x, 785.F + m_y}});
  m_wires[28].vertices->append(sf::Vertex {{725.F + m_x, 700.F + m_y}});
  m_wires[28].vertices->append(sf::Vertex {{750.F + m_x, 700.F + m_y}});
  m_wires[28].number = "28";

  // m_jumpMux -> m_pcBox
  m_wires[29].vertices->append(sf::Vertex {{1475.F + m_x, 115.F + m_y}});
  m_wires[29].vertices->append(sf::Vertex {{1500.F + m_x, 115.F + m_y}});
  m_wires[29].vertices->append(sf::Vertex {{1500.F + m_x, 20.F + m_y}});
  m_wires[29].vertices->append(sf::Vertex {{50.F + m_x, 20.F + m_y}});
  m_wires[29].vertices->append(sf::Vertex {{50.F + m_x, 535.F + m_y}});
  m_wires[29].vertices->append(sf::Vertex {{100.F + m_x, 535.F + m_y}});
  m_wires[29].number = "29";

  // m_signExt -> m_dataMux - 0
  m_wires[30].vertices->append(sf::Vertex {{950.F + m_x, 680.F + m_y}});
  m_wires[30].vertices->append(sf::Vertex {{1050.F + m_x, 680.F + m_y}});
  m_wires[30].number = "30";

  // RegDst -> m_regMux - Control
  m_wires[31].vertices->append(sf::Vertex {{800.F + m_x, 240.F + m_y}});
  m_wires[31].vertices->append(sf::Vertex {{900.F + m_x, 240.F + m_y}});
  m_wires[31].vertices->append(sf::Vertex {{900.F + m_x, 190.F + m_y}});
  m_wires[31].vertices->append(sf::Vertex {{500.F + m_x, 190.F + m_y}});
  m_wires[31].vertices->append(sf::Vertex {{500.F + m_x, 710.F + m_y}});
  m_wires[31].vertices->append(sf::Vertex {{690.F + m_x, 710.F + m_y}});
  m_wires[31].vertices->append(sf::Vertex {{690.F + m_x, 685.F + m_y}});
  m_wires[31].number = "31";
  m_wires[31].no_arrow = true;
  m_wires[31].color = kCONTROL_WIRE_COLOR;

  // Jump -> m_jumpMux - Control
  m_wires[32].vertices->append(sf::Vertex {{800.F + m_x, 265.F + m_y}});
  m_wires[32].vertices->append(sf::Vertex {{925.F + m_x, 265.F + m_y}});
  m_wires[32].vertices->append(sf::Vertex {{925.F + m_x, 35.F + m_y}});
  m_wires[32].vertices->append(sf::Vertex {{1465.F + m_x, 35.F + m_y}});
  m_wires[32].vertices->append(sf::Vertex {{1465.F + m_x, 80.F + m_y}});
  m_wires[32].number = "32";
  m_wires[32].no_arrow = true;
  m_wires[32].color = kCONTROL_WIRE_COLOR;

  // Branch -> m_andGate - Top
  m_wires[33].vertices->append(sf::Vertex {{810.F + m_x, 290.F + m_y}});
  m_wires[33].vertices->append(sf::Vertex {{1200.F + m_x, 290.F + m_y}});
  m_wires[33].vertices->append(sf::Vertex {{1200.F + m_x, 215.F + m_y}});
  m_wires[33].vertices->append(sf::Vertex {{1300.F + m_x, 215.F + m_y}});
  m_wires[33].number = "33";
  m_wires[33].no_arrow = true;
  m_wires[33].color = kCONTROL_WIRE_COLOR;

  // m_andGate -> m_branchMux - Control
  m_wires[34].vertices->append(sf::Vertex {{1350.F + m_x, 225.F + m_y}});
  m_wires[34].vertices->append(sf::Vertex {{1365.F + m_x, 225.F + m_y}});
  m_wires[34].vertices->append(sf::Vertex {{1365.F + m_x, 160.F + m_y}});
  m_wires[34].number = "34";
  m_wires[34].no_arrow = true;
  m_wires[34].color = kCONTROL_WIRE_COLOR;

  // MemRead -> m_dataMemory - Bottom Control
  m_wires[35].vertices->append(sf::Vertex {{820.F + m_x, 315.F + m_y}});
  m_wires[35].vertices->append(sf::Vertex {{1625.F + m_x, 315.F + m_y}});
  m_wires[35].vertices->append(sf::Vertex {{1625.F + m_x, 770.F + m_y}});
  m_wires[35].vertices->append(sf::Vertex {{1425.F + m_x, 770.F + m_y}});
  m_wires[35].vertices->append(sf::Vertex {{1425.F + m_x, 740.F + m_y}});
  m_wires[35].number = "35";
  m_wires[35].no_arrow = true;
  m_wires[35].color = kCONTROL_WIRE_COLOR;

  // MemtoReg -> m_writeMux - Control
  m_wires[36].vertices->append(sf::Vertex {{820.F + m_x, 340.F + m_y}});
  m_wires[36].vertices->append(sf::Vertex {{1565.F + m_x, 340.F + m_y}});
  m_wires[36].vertices->append(sf::Vertex {{1565.F + m_x, 610.F + m_y}});
  m_wires[36].number = "36";
  m_wires[36].no_arrow = true;
  m_wires[36].color = kCONTROL_WIRE_COLOR;

  // ALUOp -> m_ALUControl - Control
  m_wires[37].vertices->append(sf::Vertex {{810.F + m_x, 365.F + m_y}});
  m_wires[37].vertices->append(sf::Vertex {{975.F + m_x, 365.F + m_y}});
  m_wires[37].vertices->append(sf::Vertex {{975.F + m_x, 910.F + m_y}});
  m_wires[37].vertices->append(sf::Vertex {{1150.F + m_x, 910.F + m_y}});
  m_wires[37].vertices->append(sf::Vertex {{1150.F + m_x, 900.F + m_y}});
  m_wires[37].number = "37";
  m_wires[37].no_arrow = true;
  m_wires[37].color = kCONTROL_WIRE_COLOR;

  // m_ALUControl -> m_dataAlu - Control
  m_wires[38].vertices->append(sf::Vertex {{1200.F + m_x, 850.F + m_y}});
  m_wires[38].vertices->append(sf::Vertex {{1225.F + m_x, 850.F + m_y}});
  m_wires[38].vertices->append(sf::Vertex {{1225.F + m_x, 650.F + m_y}});
  m_wires[38].number = "38";
  m_wires[38].no_arrow = true;
  m_wires[38].color = kCONTROL_WIRE_COLOR;

  // MemWrite -> m_dataMemory - Top Control
  m_wires[39].vertices->append(sf::Vertex {{810.F + m_x, 390.F + m_y}});
  m_wires[39].vertices->append(sf::Vertex {{1425.F + m_x, 390.F + m_y}});
  m_wires[39].vertices->append(sf::Vertex {{1425.F + m_x, 550.F + m_y}});
  m_wires[39].number = "39";
  m_wires[39].no_arrow = true;
  m_wires[39].color = kCONTROL_WIRE_COLOR;

  // ALUSrc -> m_dataMux - Control
  m_wires[40].vertices->append(sf::Vertex {{800.F + m_x, 415.F + m_y}});
  m_wires[40].vertices->append(sf::Vertex {{1065.F + m_x, 415.F + m_y}});
  m_wires[40].vertices->append(sf::Vertex {{1065.F + m_x, 600.F + m_y}});
  m_wires[40].number = "40";
  m_wires[40].no_arrow = true;
  m_wires[40].color = kCONTROL_WIRE_COLOR;

  // RegWrite -> m_registers - Control
  m_wires[41].vertices->append(sf::Vertex {{800.F + m_x, 440.F + m_y}});
  m_wires[41].vertices->append(sf::Vertex {{885.F + m_x, 440.F + m_y}});
  m_wires[41].vertices->append(sf::Vertex {{885.F + m_x, 500.F + m_y}});
  m_wires[41].number = "41";
  m_wires[41].no_arrow = true;
  m_wires[41].color = kCONTROL_WIRE_COLOR;
}

void DatapathView::setupWireLabels() {
  // 4
  m_wireLabels[0] = {2, "4", true, {280.F + m_x, 150.F + m_y}};
  // Instruction [25-0]
  m_wireLabels[1] = {10, "Instruction [25-0]", true, {450.F + m_x, 50.F + m_y}};
  // Jump Address [31-0]
  m_wireLabels[2] = {
      11, "Jump Address [31-0]", true, {800.F + m_x, 60.F + m_y}};
  // 26
  m_wireLabels[3] = {10, "26", true, {460.F + m_x, 90.F + m_y}};
  // 28
  m_wireLabels[4] = {11, "28", true, {575.F + m_x, 90.F + m_y}};
  // PC + 4 [31-28]
  m_wireLabels[5] = {20, "PC + 4 [31-28]", true, {740.F + m_x, 100.F + m_y}};
  // Instruction [31-26]
  m_wireLabels[6] = {
      3, "Instruction [31-26]", true, {620.F + m_x, 330.F + m_y}};
  // Instruction [25-21]
  m_wireLabels[7] = {
      4, "Instruction [25-21]", true, {620.F + m_x, 500.F + m_y}};
  // Instruction [20-16]
  m_wireLabels[8] = {
      6, "Instruction [20-16]", true, {620.F + m_x, 565.F + m_y}};
  // Instruction [15-11]
  m_wireLabels[9] = {
      8, "Instruction [15-11]", true, {620.F + m_x, 667.F + m_y}};
  // Instruction [15-0]
  m_wireLabels[10] = {
      5, "Instruction [15-0]", true, {650.F + m_x, 840.F + m_y}};
  // Instruction [5-0]
  m_wireLabels[11] = {
      12, "Instruction [5-0]", true, {750.F + m_x, 915.F + m_y}};
  // 16
  m_wireLabels[12] = {5, "16", true, {775.F + m_x, 835.F + m_y}};
  // 32
  m_wireLabels[13] = {17, "32", true, {925.F + m_x, 835.F + m_y}};
  // RegDst
  m_wireLabels[14] = {31, "RegDst", true, {860.F + m_x, 230.F + m_y}};
  // Jump
  m_wireLabels[15] = {32, "Jump", true, {860.F + m_x, 255.F + m_y}};
  // Branch
  m_wireLabels[16] = {33, "Branch", true, {860.F + m_x, 280.F + m_y}};
  // MemRead
  m_wireLabels[17] = {35, "MemRead", true, {860.F + m_x, 305.F + m_y}};
  // MemtoReg
  m_wireLabels[18] = {36, "MemtoReg", true, {860.F + m_x, 330.F + m_y}};
  // ALUOp
  m_wireLabels[19] = {37, "ALUOp", true, {860.F + m_x, 355.F + m_y}};
  // MemWrite
  m_wireLabels[20] = {39, "MemWrite", true, {860.F + m_x, 380.F + m_y}};
  // ALUSrc
  m_wireLabels[21] = {40, "ALUSrc", true, {860.F + m_x, 405.F + m_y}};
  // RegWrite
  m_wireLabels[22] = {41, "RegWrite", true, {860.F + m_x, 430.F + m_y}};
}

// TODO(evice):  Make similar function for sf::Text
void DatapathView::drawLabel(sf::RenderWindow& window, WireLabel& wireLable) {
  sf::Text label(m_font, wireLable.label, 12);
  label.setFillColor(sf::Color::Black);
  label.setPosition(wireLable.position);
  centerText(label);
  window.draw(label);
}

} // namespace ez_arch
