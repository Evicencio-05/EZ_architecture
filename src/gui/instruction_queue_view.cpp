#include "gui/instruction_queue_view.hpp"
#include "gui/style.hpp"
#include <sstream>

namespace ez_arch {

InstructionQueueView::InstructionQueueView(sf::Font& font) : m_font(font) {}

void InstructionQueueView::setPosition(float x, float y) { m_x = x; m_y = y; }
void InstructionQueueView::setSize(float w, float h) { m_w = w; m_h = h; }
void InstructionQueueView::setItems(const std::vector<std::string>& items) { m_items = items; }

void InstructionQueueView::handleMouseMove(float, float) {}

static bool within(float x, float y, float rx, float ry, float rw, float rh) {
  return x >= rx && x <= rx + rw && y >= ry && y <= ry + rh;
}

void InstructionQueueView::handleMousePress(float x, float y) {
  // Detect Clear button press
  float bx = m_x + 20; float by = m_y + m_h - 40; float bw = 100; float bh = 24;
  if (within(x, y, bx, by, bw, bh)) {
    m_clearPressed = true;
  }
  // Detect row buttons
  const float rowStartY = m_y + 50;
  const float rowH = 20;
  for (size_t i = 0; i < m_items.size(); ++i) {
    float baseY = rowStartY + i * rowH;
    // X button
    if (within(x, y, m_x + m_w - 30, baseY, 20, 18)) m_pressedDelete = static_cast<int>(i);
    // Up button
    if (within(x, y, m_x + m_w - 56, baseY, 20, 18)) m_pressedMoveUp = static_cast<int>(i);
    // Down button
    if (within(x, y, m_x + m_w - 82, baseY, 20, 18)) m_pressedMoveDown = static_cast<int>(i);
  }
}

void InstructionQueueView::handleMouseRelease(float x, float y) {
  float bx = m_x + 20; float by = m_y + m_h - 40; float bw = 100; float bh = 24;
  bool clearWas = m_clearPressed; m_clearPressed = false;
  if (clearWas && within(x, y, bx, by, bw, bh)) {
    if (m_onClear) m_onClear();
  }

  int upIdx = m_pressedMoveUp; m_pressedMoveUp = -1;
  if (upIdx >= 0) {
    float baseY = m_y + 50 + upIdx * 20;
    if (within(x, y, m_x + m_w - 56, baseY, 20, 18)) {
      if (m_onMove) m_onMove(static_cast<size_t>(upIdx), -1);
    }
  }

  int downIdx = m_pressedMoveDown; m_pressedMoveDown = -1;
  if (downIdx >= 0) {
    float baseY = m_y + 50 + downIdx * 20;
    if (within(x, y, m_x + m_w - 82, baseY, 20, 18)) {
      if (m_onMove) m_onMove(static_cast<size_t>(downIdx), +1);
    }
  }

  int idx = m_pressedDelete; m_pressedDelete = -1;
  if (idx >= 0) {
    float baseY = m_y + 50 + idx * 20;
    if (within(x, y, m_x + m_w - 30, baseY, 20, 18)) {
      if (m_onDelete) m_onDelete(static_cast<size_t>(idx));
    }
  }
}

void InstructionQueueView::draw(sf::RenderWindow& window) {
  drawPanel(window);

  // Title
  sf::Text title(m_font);
  title.setString("Instruction Queue");
  title.setCharacterSize(16);
  title.setFillColor(sf::Color::Black);
  title.setPosition({m_x + 12, m_y + 12});
  window.draw(title);

  // Rows
  const float rowStartY = m_y + 50;
  const float rowH = 20;
  for (size_t i = 0; i < m_items.size(); ++i) {
    // Text
    sf::Text t(m_font);
    t.setString(m_items[i]);
    t.setCharacterSize(14);
    t.setFillColor(sf::Color::Black);
    t.setPosition({m_x + 16, rowStartY + i * rowH});
    window.draw(t);

    // Down button
    sf::RectangleShape dbtn({20, 18});
    dbtn.setPosition({m_x + m_w - 82, rowStartY + i * rowH});
    dbtn.setFillColor(BUTTON_NORMAL_COLOR);
    dbtn.setOutlineColor(sf::Color::Black);
    dbtn.setOutlineThickness(1.f);
    window.draw(dbtn);
    sf::Text dt(m_font);
    dt.setString("v");
    dt.setCharacterSize(12);
    dt.setFillColor(BUTTON_TEXT_COLOR);
    dt.setPosition({m_x + m_w - 76, rowStartY + i * rowH + 1});
    window.draw(dt);

    // Up button
    sf::RectangleShape ubtn({20, 18});
    ubtn.setPosition({m_x + m_w - 56, rowStartY + i * rowH});
    ubtn.setFillColor(BUTTON_NORMAL_COLOR);
    ubtn.setOutlineColor(sf::Color::Black);
    ubtn.setOutlineThickness(1.f);
    window.draw(ubtn);
    sf::Text ut(m_font);
    ut.setString("^");
    ut.setCharacterSize(12);
    ut.setFillColor(BUTTON_TEXT_COLOR);
    ut.setPosition({m_x + m_w - 50, rowStartY + i * rowH + 0});
    window.draw(ut);

    // X button
    sf::RectangleShape xbtn({20, 18});
    xbtn.setPosition({m_x + m_w - 30, rowStartY + i * rowH});
    xbtn.setFillColor(BUTTON_NORMAL_COLOR);
    xbtn.setOutlineColor(sf::Color::Black);
    xbtn.setOutlineThickness(1.f);
    window.draw(xbtn);

    sf::Text xt(m_font);
    xt.setString("X");
    xt.setCharacterSize(12);
    xt.setFillColor(BUTTON_TEXT_COLOR);
    xt.setPosition({m_x + m_w - 24, rowStartY + i * rowH + 2});
    window.draw(xt);
  }

  // Clear button
  sf::RectangleShape clr({100, 24});
  clr.setPosition({m_x + 20, m_y + m_h - 40});
  clr.setFillColor(m_clearPressed ? BUTTON_PRESSED_COLOR : BUTTON_NORMAL_COLOR);
  clr.setOutlineColor(sf::Color::Black);
  clr.setOutlineThickness(1.f);
  window.draw(clr);

  sf::Text clrt(m_font);
  clrt.setString("Clear");
  clrt.setCharacterSize(14);
  clrt.setFillColor(BUTTON_TEXT_COLOR);
  clrt.setPosition({m_x + 50, m_y + m_h - 38});
  window.draw(clrt);
}

void InstructionQueueView::drawPanel(sf::RenderWindow& window) {
  sf::RectangleShape bg({m_w, m_h});
  bg.setPosition({m_x, m_y});
  bg.setFillColor(VIEW_BOX_BACKGROUND_COLOR);
  bg.setOutlineColor(VIEW_BOX_OUTLINE_COLOR);
  bg.setOutlineThickness(2.f);
  window.draw(bg);
}

} // namespace ez_arch
