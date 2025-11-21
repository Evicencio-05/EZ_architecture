#include "gui/register_view.hpp"
#include "core/register_names.hpp"
#include "gui/style.hpp"
#include <iomanip>
#include <sstream>

namespace ez_arch {

RegisterView::RegisterView(const RegisterFile& registers, sf::Font& font)
    : m_registers(registers), m_font(font) {
}

void RegisterView::set_position(float x, float y) {
    m_x = x;
    m_y = y;
}

void RegisterView::update() {
    // For now, nothing to do here
    // In the future, we could detect which registers changed and highlight them
}

void RegisterView::draw(sf::RenderWindow& window) {
    // Draw the header (title + column labels)
    draw_header(window);
    
    // Draw all 32 registers in a table
    const float kROW_HEIGHT = 17.F;
    const float kHEADER_HEIGHT = 60.F;
    
    for (int i = 0; i < 32; i++) {
        float reg_y = m_y + kHEADER_HEIGHT + (i * kROW_HEIGHT);
        draw_register(window, i, reg_y);
    }
}

void RegisterView::draw_header(sf::RenderWindow& window) {
    // Draw background box for the entire register view
    sf::RectangleShape background({180.F, 610.F});
    background.setPosition({m_x, m_y});
    background.setFillColor(kVIEW_BOX_BACKGROUND_COLOR);
    background.setOutlineColor(kVIEW_BOX_OUTLINE_COLOR);
    background.setOutlineThickness(2.F);
    window.draw(background);
    
    // Draw title
    sf::Text title(m_font);
    title.setString("Registers");
    title.setCharacterSize(20);
    title.setFillColor(kTITLE_TEXT_COLOR);
    title.setPosition({m_x + 10.F, m_y + 10.F});
    window.draw(title);
    
    // Draw column headers
    sf::Text header(m_font);
    header.setString("Reg    Name   Value");
    header.setCharacterSize(14);
    header.setFillColor(sf::Color::Black);
    header.setPosition({m_x + 10.F, m_y + 40.F});
    window.draw(header);
}

void RegisterView::draw_register(sf::RenderWindow& window, int reg_num, float row_y) {
    // Read the register value
    word_t value = m_registers.read(reg_num);
    
    // Get register name
    std::string_view reg_name = kREGISTER_NAMES[reg_num];
    
    // Format register number (e.g., "$0", "$1", etc.)
    std::ostringstream reg_num_stream;
    reg_num_stream << reg_num;
    
    // Format value as hexadecimal (e.g., "0x00000000")
    std::ostringstream value_stream;
    value_stream << "0x" << std::hex << std::setw(8) << std::setfill('0') << value;
    
    // Draw register number
    sf::Text reg_num_text(m_font);
    reg_num_text.setString(reg_num_stream.str());
    reg_num_text.setCharacterSize(12);
    reg_num_text.setFillColor(sf::Color::Black);
    reg_num_text.setPosition({m_x + 10.F, row_y});
    window.draw(reg_num_text);
    
    // Draw register name
    sf::Text name_text(m_font);
    name_text.setString(std::string(reg_name));
    name_text.setCharacterSize(12);
    name_text.setFillColor(sf::Color::Black);
    name_text.setPosition({m_x + 50.F, row_y});
    window.draw(name_text);
    
    // Draw register value
    sf::Text value_text(m_font);
    value_text.setString(value_stream.str());
    value_text.setCharacterSize(12);
    value_text.setFillColor(sf::Color::Black);
    value_text.setPosition({m_x + 100.F, row_y});
    window.draw(value_text);
}

} // namespace ez_arch
