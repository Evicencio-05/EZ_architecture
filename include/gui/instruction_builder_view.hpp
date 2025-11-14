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

  InstructionBuilderView(sf::Font& font);

  void setPosition(float x, float y);
  void setSize(float w, float h);

  // Event handling (use like Button)
  void handleMouseMove(float x, float y);
  void handleMousePress(float x, float y);
  void handleMouseRelease(float x, float y);
  void handleMouseWheel(float x, float y, float delta);
  
  // Keyboard/text input
  void handleTextEntered(uint32_t codepoint);
  void handleKeyPressed(int keyCode); // sf::Keyboard::Key as int to avoid header coupling

  void draw(sf::RenderWindow& window);

  // Set callback when user clicks Add
  void setOnAdd(OnAddCallback cb) { m_onAdd = std::move(cb); }

  // State queries and controls for integration (hotkeys)
  bool anyDropdownOpen() const { return m_opOpen || m_rsOpen || m_rtOpen || m_rdOpen; }
  bool textActive() const { return m_immActive; }
  void openOp() { m_opOpen = true; m_rsOpen = m_rtOpen = m_rdOpen = false; }
  void openRS() { m_rsOpen = true; m_opOpen = m_rtOpen = m_rdOpen = false; }
  void openRT() { m_rtOpen = true; m_opOpen = m_rsOpen = m_rdOpen = false; }
  void openRD() { m_rdOpen = true; m_opOpen = m_rsOpen = m_rtOpen = false; }
  void closeDropdowns() { m_opOpen = m_rsOpen = m_rtOpen = m_rdOpen = false; }

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
  void drawPanel(sf::RenderWindow& window);
  void drawLabel(sf::RenderWindow& window, const std::string& text, float x, float y, unsigned size=14);
  void drawTextBox(sf::RenderWindow& window, const std::string& value, float x, float y, float w, float h, bool active=false);
  void drawButton(sf::RenderWindow& window, const std::string& text, float x, float y, float w, float h, bool pressed);

  // Assemble from current selection
  std::string currentAssembly() const;
};

} // namespace ez_arch
