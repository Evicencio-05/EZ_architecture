#pragma once

#include <SFML/Graphics.hpp>

namespace ez_arch {

// View box styling
constexpr sf::Color kVIEW_BOX_BACKGROUND_COLOR(220, 220, 220);
constexpr sf::Color kVIEW_BOX_OUTLINE_COLOR(sf::Color::Black);
constexpr float kVIEW_BOX_OUTLINE_THICKNESS = 2.F;
constexpr sf::Color kDATA_PATH_BACKGROUND_COLOR(230, 230, 230);

// Text colors
constexpr sf::Color kTITLE_TEXT_COLOR(sf::Color::Black);
constexpr sf::Color kHEADER_TEXT_COLOR(60, 60, 60);
constexpr sf::Color kNORMAL_TEXT_COLOR(80, 80, 80);
constexpr sf::Color kVALUE_TEXT_COLOR(50, 150, 50); // Green for values

// Highlighting
constexpr sf::Color kPC_HIGHLIGHT_COLOR(255, 255, 100); // Yellow for PC
constexpr sf::Color kPC_HIGHLIGHT_OUTLINE(200, 200, 0);

// Wire Color
constexpr sf::Color kCONTROL_WIRE_COLOR(20, 170, 210);

// Sizes
constexpr unsigned int kTITLE_FONT_SIZE = 20;
constexpr unsigned int kHEADER_FONT_SIZE = 14;
constexpr unsigned int kNORMAL_FONT_SIZE = 12;

constexpr float kROW_HEIGHT = 22.F;
constexpr float kPADDING = 10.F;

// Button styling
constexpr sf::Color kBUTTON_NORMAL_COLOR(100, 150, 200);
constexpr sf::Color kBUTTON_HOVER_COLOR(120, 170, 220);
constexpr sf::Color kBUTTON_PRESSED_COLOR(80, 130, 180);
constexpr sf::Color kBUTTON_TEXT_COLOR(255, 255, 255);
constexpr float kBUTTON_WIDTH = 80.F;
constexpr float kBUTTON_HEIGHT = 35.F;

// Layout constants
constexpr float kTOP_BAR_HEIGHT = 60.F;
constexpr float kLEFT_SIDEBAR_WIDTH = 60.F;
constexpr float kTOGGLE_BUTTON_SIZE = 50.F;

} // namespace ez_arch
