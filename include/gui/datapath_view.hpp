#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "gui/custom_shapes.hpp"
#include "core/cpu.hpp"

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
        sf::Vector2f getLeft(size_t inputPos, float characterSize) const { 
          float yOffsetScale = static_cast<float>(((inputPos * 2) + 1) / static_cast<float>(inputs.size() * 2));
          return {position.x + 5.f, position.y + (size.y * yOffsetScale - characterSize)};
        }
        sf::Vector2f getRight(size_t outputPos, float characterSize, float xOffset = 0) const {
          float yOffsetScale = static_cast<float>(((outputPos * 2) + 1) / static_cast<float>(4 * 2));
          return {position.x + (size.x / 2.f) + xOffset, position.y + (size.y * yOffsetScale - characterSize)}; 
        }
        sf::Vector2f getTop() const { 
          return {position.x + size.x / 2.f, position.y}; 
        }
        sf::Vector2f getBottom() const 
        { 
          return {position.x + size.x / 2.f, position.y + size.y}; 
        }
        sf::Vector2f getCenter() const 
        { 
          return {position.x + size.x / 2.f, position.y + size.y / 2.f}; 
        }
    };

    // Wire/Connection definition
    struct Wire {
        sf::Vector2f start;
        sf::Vector2f end;
        std::string label;
        sf::Color color;
        bool active;  // Highlight when active
        
        Wire(sf::Vector2f s, sf::Vector2f e, std::string lbl = "", 
             sf::Color col = sf::Color(150, 150, 150), bool act = false)
            : start(s), end(e), label(lbl), color(col), active(act) {}
    };
    
    ComponentBox m_pcBox;
    ComponentBox m_instructionMemory;
    ComponentBox m_registers;
    ComponentBox m_dataMemory;

    std::unique_ptr<EllipseShape> m_signExt;
    std::unique_ptr<EllipseShape> m_ALUControl;
    std::unique_ptr<EllipseShape> m_jumpSL;
    std::unique_ptr<EllipseShape> m_branchSL;


    
    // All wire connections
    std::vector<Wire> m_wires;
    
    // Drawing helpers
    void calculateLayout();
    void setupWires();  // Define all wire connections
    void drawComponentBox(sf::RenderWindow& window, const ComponentBox& box, sf::Color color);
    void drawEllipse(sf::RenderWindow& window, std::unique_ptr<EllipseShape>& circle, sf::Color color);
    void drawWire(sf::RenderWindow& window, const Wire& wire);
    void drawWireLabel(sf::RenderWindow& window, const Wire& wire);
    void centerText(sf::Text& text);
};

} // namespace ez_arch
