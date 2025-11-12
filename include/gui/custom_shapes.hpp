#pragma once

#include <cmath>
#include <string>
#include <SFML/Graphics.hpp>

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

// class ALUShape : public sf::Drawable, public sf::Transformable {
// public:
//     ALUShape(sf::Color color = sf::Color::White) : m_vertices(sf::PrimitiveType::Triangles, 15), m_fillColor(color) {
//         // Define the 7 points of the concave polygon (clockwise order)
//         sf::Vector2f points[7] = {
//             sf::Vector2f(60.f, 0.f),    // p0: top left
//             sf::Vector2f(140.f, 0.f),   // p1: top right
//             sf::Vector2f(200.f, 100.f), // p2: bottom right
//             sf::Vector2f(110.f, 100.f), // p3: indent end
//             sf::Vector2f(100.f, 80.f),  // p4: indent tip (pointing upward into the shape)
//             sf::Vector2f(90.f, 100.f),  // p5: indent start
//             sf::Vector2f(0.f, 100.f)    // p6: bottom left
//         };
//
//         // Triangulate the concave polygon into 5 non-overlapping triangles
//         // that cover the interior (trapezoid minus the triangular indent)
//         int idx = 0;
//         // Triangle 1: p0, p6, p5
//         m_vertices[idx++].position = points[0];
//         m_vertices[idx++].position = points[6];
//         m_vertices[idx++].position = points[5];
//         // Triangle 2: p0, p5, p4
//         m_vertices[idx++].position = points[0];
//         m_vertices[idx++].position = points[5];
//         m_vertices[idx++].position = points[4];
//         // Triangle 3: p0, p4, p3
//         m_vertices[idx++].position = points[0];
//         m_vertices[idx++].position = points[4];
//         m_vertices[idx++].position = points[3];
//         // Triangle 4: p0, p3, p2
//         m_vertices[idx++].position = points[0];
//         m_vertices[idx++].position = points[3];
//         m_vertices[idx++].position = points[2];
//         // Triangle 5: p0, p2, p1
//         m_vertices[idx++].position = points[0];
//         m_vertices[idx++].position = points[2];
//         m_vertices[idx++].position = points[1];
//
//         updateColors();
//     }
//
//     void setFillColor(const sf::Color& color) {
//         m_fillColor = color;
//         updateColors();
//     }
//
//     void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
//         states.transform *= getTransform();
//         target.draw(m_vertices, states);
//     }
//
// private:
//     sf::VertexArray m_vertices;
//     sf::Color m_fillColor;
//
//     void updateColors() {
//       for (auto& vertex : m_vertices) {
//         vertex.color = m_fillColor;
//       }
//     }
//
// };

} // namesape ez_arch
