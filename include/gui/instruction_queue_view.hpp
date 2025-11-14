#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <functional>

namespace ez_arch {

// Read-only list of instructions with per-row delete (X) button and clear-all.
// Also supports reordering via Up/Down buttons.
class InstructionQueueView {
public:
  using OnDeleteCallback = std::function<void(size_t)>; // delete index
  using OnClearCallback = std::function<void()>;
  using OnMoveCallback = std::function<void(size_t, int)>; // index, delta (-1 up, +1 down)

  InstructionQueueView(sf::Font& font);

  void setPosition(float x, float y);
  void setSize(float w, float h);

  void setItems(const std::vector<std::string>& items);

  void handleMouseMove(float x, float y);
  void handleMousePress(float x, float y);
  void handleMouseRelease(float x, float y);

  void draw(sf::RenderWindow& window);

  void setOnDelete(OnDeleteCallback cb) { m_onDelete = std::move(cb); }
  void setOnClear(OnClearCallback cb) { m_onClear = std::move(cb); }
  void setOnMove(OnMoveCallback cb) { m_onMove = std::move(cb); }

private:
  sf::Font& m_font;
  float m_x{0}, m_y{0}, m_w{500}, m_h{300};
  std::vector<std::string> m_items;

  bool m_clearPressed{false};
  int m_pressedDelete{-1};
  int m_pressedMoveUp{-1};
  int m_pressedMoveDown{-1};

  OnDeleteCallback m_onDelete;
  OnClearCallback m_onClear;
  OnMoveCallback m_onMove;

  void drawPanel(sf::RenderWindow& window);
};

} // namespace ez_arch
