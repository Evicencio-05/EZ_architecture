#pragma once

#include <SFML/Graphics.hpp>
#include "core/cpu.hpp"

namespace ez_arch {

class DatapathView {
public:
    DatapathView(const CPU& cpu, sf::Font& font);
    
    void setPosition(float x, float y);
    void setSize(float width, float height);
    
    void update();
    void draw(sf::RenderWindow& window);
    
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
        
        // Helper to get connection points
        sf::Vector2f getLeft() const { return {position.x, position.y + size.y / 2.f}; }
        sf::Vector2f getRight() const { return {position.x + size.x, position.y + size.y / 2.f}; }
        sf::Vector2f getTop() const { return {position.x + size.x / 2.f, position.y}; }
        sf::Vector2f getBottom() const { return {position.x + size.x / 2.f, position.y + size.y}; }
        sf::Vector2f getCenter() const { return {position.x + size.x / 2.f, position.y + size.y / 2.f}; }
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
    ComponentBox m_instrMemBox;
    ComponentBox m_regFileBox;
    ComponentBox m_aluBox;
    ComponentBox m_dataMemBox;
    ComponentBox m_controlBox;
    
    // All wire connections
    std::vector<Wire> m_wires;
    
    // Drawing helpers
    void calculateLayout();
    void setupWires();  // Define all wire connections
    void drawComponent(sf::RenderWindow& window, const ComponentBox& box, sf::Color color);
    void drawWire(sf::RenderWindow& window, const Wire& wire);
    void drawWireLabel(sf::RenderWindow& window, const Wire& wire);
    void drawConnection(sf::RenderWindow& window, sf::Vector2f start, sf::Vector2f end, 
                       sf::Color color, bool active = false);
    
    // Helper for drawing multiplexers
    void drawMux(sf::RenderWindow& window, sf::Vector2f position, float size);
};

} // namespace ez_arch
