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

void DatapathView::calculateLayout() {
    // Layout the datapath components in a logical flow (left to right)
    
    // PC (top left)
    m_pcBox.position = {m_x + 50.f, m_y + 50.f};
    m_pcBox.size = {60.f, 40.f};
    m_pcBox.label = "PC";
    
    // Instruction Memory (after PC)
    m_instrMemBox.position = {m_x + 150.f, m_y + 30.f};
    m_instrMemBox.size = {100.f, 80.f};
    m_instrMemBox.label = "Instruction\nMemory";
    
    // Control Unit (top center)
    m_controlBox.position = {m_x + 300.f, m_y + 30.f};
    m_controlBox.size = {120.f, 60.f};
    m_controlBox.label = "Control Unit";
    
    // Register File (middle left)
    m_regFileBox.position = {m_x + 150.f, m_y + 200.f};
    m_regFileBox.size = {120.f, 100.f};
    m_regFileBox.label = "Register\nFile";
    
    // ALU (middle center)
    m_aluBox.position = {m_x + 400.f, m_y + 220.f};
    m_aluBox.size = {80.f, 60.f};
    m_aluBox.label = "ALU";
    
    // Data Memory (middle right)
    m_dataMemBox.position = {m_x + 550.f, m_y + 200.f};
    m_dataMemBox.size = {100.f, 100.f};
    m_dataMemBox.label = "Data\nMemory";
}

void DatapathView::setupWires() {
    m_wires.clear();
    
    // PC to Instruction Memory
    m_wires.emplace_back(
        m_pcBox.getRight(), 
        m_instrMemBox.getLeft(),
        "PC",
        sf::Color(100, 200, 255)
    );
    
    // Instruction Memory to Control Unit (opcode)
    m_wires.emplace_back(
        sf::Vector2f{m_instrMemBox.getRight().x, m_instrMemBox.getTop().y + 20.f},
        m_controlBox.getLeft(),
        "Opcode",
        sf::Color(200, 150, 100)
    );
    
    // Instruction Memory to Register File (register addresses)
    m_wires.emplace_back(
        m_instrMemBox.getBottom(),
        m_regFileBox.getTop(),
        "Rs, Rt, Rd",
        sf::Color(150, 150, 200)
    );
    
    // Register File to ALU (Read Data 1)
    m_wires.emplace_back(
        sf::Vector2f{m_regFileBox.getRight().x, m_regFileBox.getTop().y + 30.f},
        sf::Vector2f{m_aluBox.getLeft().x, m_aluBox.getTop().y + 15.f},
        "Read Data 1",
        sf::Color(100, 200, 150)
    );
    
    // Register File to ALU (Read Data 2)
    m_wires.emplace_back(
        sf::Vector2f{m_regFileBox.getRight().x, m_regFileBox.getBottom().y - 30.f},
        sf::Vector2f{m_aluBox.getLeft().x, m_aluBox.getBottom().y - 15.f},
        "Read Data 2",
        sf::Color(100, 200, 150)
    );
    
    // ALU to Data Memory (address/data)
    m_wires.emplace_back(
        m_aluBox.getRight(),
        m_dataMemBox.getLeft(),
        "ALU Result",
        sf::Color(200, 100, 100)
    );
    
    // Data Memory back to Register File (write back path)
    // This one goes around the outside
    sf::Vector2f memOutTop = {m_dataMemBox.getTop().x, m_dataMemBox.getTop().y - 20.f};
    sf::Vector2f regInTop = {m_regFileBox.getTop().x + 30.f, m_regFileBox.getTop().y - 20.f};
    
    m_wires.emplace_back(
        m_dataMemBox.getTop(),
        memOutTop,
        "",
        sf::Color(150, 150, 200)
    );
    
    m_wires.emplace_back(
        memOutTop,
        regInTop,
        "Write Data",
        sf::Color(150, 150, 200)
    );
    
    m_wires.emplace_back(
        regInTop,
        sf::Vector2f{m_regFileBox.getTop().x + 30.f, m_regFileBox.getTop().y},
        "",
        sf::Color(150, 150, 200)
    );
}

void DatapathView::update() {
    // Update will be used to highlight active paths based on current instruction
}

void DatapathView::draw(sf::RenderWindow& window) {
    // Draw wires first (so they appear behind components)
    for (const auto& wire : m_wires) {
        drawWire(window, wire);
    }
    
    // Draw all major components
    drawComponent(window, m_pcBox, sf::Color(100, 150, 200));
    drawComponent(window, m_instrMemBox, sf::Color(150, 100, 200));
    drawComponent(window, m_controlBox, sf::Color(200, 150, 100));
    drawComponent(window, m_regFileBox, sf::Color(100, 200, 150));
    drawComponent(window, m_aluBox, sf::Color(200, 100, 100));
    drawComponent(window, m_dataMemBox, sf::Color(150, 150, 200));
    
    // Draw wire labels on top of everything
    for (const auto& wire : m_wires) {
        if (!wire.label.empty()) {
            drawWireLabel(window, wire);
        }
    }
}

void DatapathView::drawComponent(sf::RenderWindow& window, const ComponentBox& box, sf::Color color) {
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

void DatapathView::drawConnection(sf::RenderWindow& window, sf::Vector2f start, sf::Vector2f end,
                                   sf::Color color, bool active) {
    // Legacy method - kept for compatibility
    Wire wire(start, end, "", color, active);
    drawWire(window, wire);
}


void DatapathView::drawMux(sf::RenderWindow& window, sf::Vector2f position, float size) {
    // Draw a simple multiplexer as a triangle
    sf::ConvexShape mux;
    mux.setPointCount(3);
    mux.setPoint(0, position);
    mux.setPoint(1, {position.x + size, position.y - size / 2.f});
    mux.setPoint(2, {position.x + size, position.y + size / 2.f});
    mux.setFillColor(sf::Color(180, 180, 180));
    mux.setOutlineColor(sf::Color::White);
    mux.setOutlineThickness(1.f);
    window.draw(mux);
}

} // namespace ez_arch
