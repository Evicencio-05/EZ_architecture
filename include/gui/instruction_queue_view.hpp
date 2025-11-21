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

  explicit InstructionQueueView(sf::Font& font);

  void set_position(float x, float y);
  void set_size(float w, float h);

  void set_items(const std::vector<std::string>& items);

  void handle_mouse_move(float x, float y);
  void handle_mouse_press(float x, float y);
  void handle_mouse_release(float x, float y);

  void draw(sf::RenderWindow& window);

  void set_on_delete(OnDeleteCallback cb) { m_onDelete = std::move(cb); }
  void set_on_clear(OnClearCallback cb) { m_onClear = std::move(cb); }
  void set_on_move(OnMoveCallback cb) { m_onMove = std::move(cb); }

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

  void draw_panel(sf::RenderWindow& window) const;
};

} // namespace ez_arch
