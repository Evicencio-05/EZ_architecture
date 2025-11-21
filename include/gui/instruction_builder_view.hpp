#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <functional>

namespace ez_arch {

// A small UI to compose supported instructions with dropdowns and inputs.
// Emits validated assembly lines (e.g., "add $t0, $t1, $t2").
class InstructionBuilderView {
public:
  using OnAddCallback = std::function<void(const std::string&)>;

  explicit InstructionBuilderView(sf::Font& font);

  void set_position(float x, float y);
  void set_size(float w, float h);

  // Event handling (use like Button)
  void handle_mouse_move(float x, float y);
  void handle_mouse_press(float x, float y);
  void handle_mouse_release(float x, float y);
  void handle_mouse_wheel(float x, float y, float delta);
  
  // Keyboard/text input
  void handle_text_entered(uint32_t codepoint);
  void handle_key_pressed(int key_code); // sf::Keyboard::Key as int to avoid header coupling

  void draw(sf::RenderWindow& window);

  // Set callback when user clicks Add
  void set_on_add(OnAddCallback cb) { m_onAdd = std::move(cb); }

  // State queries and controls for integration (hotkeys)
  [[nodiscard]] bool any_dropdown_open() const { return m_opOpen || m_rsOpen || m_rtOpen || m_rdOpen; }
  [[nodiscard]] bool text_active() const { return m_immActive; }
  void open_op() { m_opOpen = true; m_rsOpen = m_rtOpen = m_rdOpen = false; }
  void open_rs() { m_rsOpen = true; m_opOpen = m_rtOpen = m_rdOpen = false; }
  void open_rt() { m_rtOpen = true; m_opOpen = m_rsOpen = m_rdOpen = false; }
  void open_rd() { m_rdOpen = true; m_opOpen = m_rsOpen = m_rtOpen = false; }
  void close_dropdowns() { m_opOpen = m_rsOpen = m_rtOpen = m_rdOpen = false; }

private:
  sf::Font& m_font;
  float m_x{0}, m_y{0}, m_w{500}, m_h{300};

  // Simplified widgets state
  int m_opIndex{0};
  int m_rs{0};
  int m_rt{0};
  int m_rd{0};
  std::string m_imm; // textual immediate
  bool m_immActive{false};

  // Dropdown open states
  bool m_opOpen{false};
  bool m_rsOpen{false};
  bool m_rtOpen{false};
  bool m_rdOpen{false};
  int m_opScroll{0};
  int m_rsScroll{0};
  int m_rtScroll{0};
  int m_rdScroll{0};

  // Error state
  std::string m_lastError;

  bool m_addPressed{false};

  OnAddCallback m_onAdd;

  // Helpers
  void draw_panel(sf::RenderWindow& window) const;
  void draw_label(sf::RenderWindow& window, const std::string& text, float x, float y, unsigned size=14);
  void draw_text_box(sf::RenderWindow& window, const std::string& value, float x, float y, float w, float h, bool active=false);
  void draw_button(sf::RenderWindow& window, const std::string& text, float x, float y, float w, float h, bool pressed);

  // Assemble from current selection
  [[nodiscard]] std::string current_assembly() const;
};

} // namespace ez_arch
