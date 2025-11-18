#pragma once

#include <SFML/Graphics.hpp>

namespace ez_arch {

// View box styling
constexpr sf::Color VIEW_BOX_BACKGROUND_COLOR(220, 220, 220);
constexpr sf::Color VIEW_BOX_OUTLINE_COLOR(sf::Color::Black);
constexpr float VIEW_BOX_OUTLINE_THICKNESS = 2.f;
constexpr sf::Color DATA_PATH_BACKGROUND_COLOR(230, 230, 230);

// Text colors
constexpr sf::Color TITLE_TEXT_COLOR(sf::Color::Black);
constexpr sf::Color HEADER_TEXT_COLOR(60, 60, 60);
constexpr sf::Color NORMAL_TEXT_COLOR(80, 80, 80);
constexpr sf::Color VALUE_TEXT_COLOR(50, 150, 50);  // Green for values

// Highlighting
constexpr sf::Color PC_HIGHLIGHT_COLOR(255, 255, 100);  // Yellow for PC
constexpr sf::Color PC_HIGHLIGHT_OUTLINE(200, 200, 0);

// Wire Color
constexpr sf::Color CONTROL_WIRE_COLOR(20, 170, 210);

// Sizes
constexpr unsigned int TITLE_FONT_SIZE = 20;
constexpr unsigned int HEADER_FONT_SIZE = 14;
constexpr unsigned int NORMAL_FONT_SIZE = 12;

constexpr float ROW_HEIGHT = 22.f;
constexpr float PADDING = 10.f;

// Button styling
constexpr sf::Color BUTTON_NORMAL_COLOR(100, 150, 200);
constexpr sf::Color BUTTON_HOVER_COLOR(120, 170, 220);
constexpr sf::Color BUTTON_PRESSED_COLOR(80, 130, 180);
constexpr sf::Color BUTTON_TEXT_COLOR(255, 255, 255);
constexpr float BUTTON_WIDTH = 80.f;
constexpr float BUTTON_HEIGHT = 35.f;

// Layout constants
constexpr float TOP_BAR_HEIGHT = 60.f;
constexpr float LEFT_SIDEBAR_WIDTH = 60.f;
constexpr float TOGGLE_BUTTON_SIZE = 50.f;

}  // namespace ez_arch
