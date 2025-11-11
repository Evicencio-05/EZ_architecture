#include "gui/register_view.hpp"
#include "core/register_names.hpp"
#include "gui/style.hpp"
#include <iomanip>
#include <sstream>

namespace ez_arch {

RegisterView::RegisterView(const RegisterFile& registers, sf::Font& font)
    : registers_(registers), font_(font), x_(0.f), y_(0.f) {
}

void RegisterView::setPosition(float x, float y) {
    x_ = x;
    y_ = y;
}

void RegisterView::update() {
    // For now, nothing to do here
    // In the future, we could detect which registers changed and highlight them
}

void RegisterView::draw(sf::RenderWindow& window) {
    // Draw the header (title + column labels)
    drawHeader(window);
    
    // Draw all 32 registers in a table
    const float COLUMN_WIDTH = 200.f;
    const float ROW_HEIGHT = 17.f;
    const float HEADER_HEIGHT = 60.f;
    
    for (int i = 0; i < 32; i++) {
        float regX = x_ + COLUMN_WIDTH;
        float regY = y_ + HEADER_HEIGHT + (i * ROW_HEIGHT);
        
        drawRegister(window, i, regY);
    }
}

void RegisterView::drawHeader(sf::RenderWindow& window) {
    // Draw background box for the entire register view
    sf::RectangleShape background({180.f, 610.f});
    background.setPosition({x_, y_});
    background.setFillColor(VIEW_BOX_BACKGROUND_COLOR);
    background.setOutlineColor(VIEW_BOX_OUTLINE_COLOR);
    background.setOutlineThickness(2.f);
    window.draw(background);
    
    // Draw title
    sf::Text title(font_);
    title.setString("Registers");
    title.setCharacterSize(20);
    title.setFillColor(TITLE_TEXT_COLOR);
    title.setPosition({x_ + 10.f, y_ + 10.f});
    window.draw(title);
    
    // Draw column headers
    sf::Text header(font_);
    header.setString("Reg    Name   Value");
    header.setCharacterSize(14);
    header.setFillColor(sf::Color::Black);
    header.setPosition({x_ + 10.f, y_ + 40.f});
    window.draw(header);
}

void RegisterView::drawRegister(sf::RenderWindow& window, int regNum, float rowY) {
    // Read the register value
    word_t value = registers_.read(regNum);
    
    // Get register name
    std::string_view regName = REGISTER_NAMES[regNum];
    
    // Format register number (e.g., "$0", "$1", etc.)
    std::ostringstream regNumStream;
    regNumStream << regNum;
    
    // Format value as hexadecimal (e.g., "0x00000000")
    std::ostringstream valueStream;
    valueStream << "0x" << std::hex << std::setw(8) << std::setfill('0') << value;
    
    // Draw register number
    sf::Text regNumText(font_);
    regNumText.setString(regNumStream.str());
    regNumText.setCharacterSize(12);
    regNumText.setFillColor(sf::Color::Black);
    regNumText.setPosition({x_ + 10.f, rowY});
    window.draw(regNumText);
    
    // Draw register name
    sf::Text nameText(font_);
    nameText.setString(std::string(regName));
    nameText.setCharacterSize(12);
    nameText.setFillColor(sf::Color::Black);
    nameText.setPosition({x_ + 50.f, rowY});
    window.draw(nameText);
    
    // Draw register value
    sf::Text valueText(font_);
    valueText.setString(valueStream.str());
    valueText.setCharacterSize(12);
    valueText.setFillColor(sf::Color::Black);
    valueText.setPosition({x_ + 100.f, rowY});
    window.draw(valueText);
}

} // namespace ez_arch
