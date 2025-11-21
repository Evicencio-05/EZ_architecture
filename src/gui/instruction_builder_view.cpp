#include "gui/instruction_builder_view.hpp"

#include "core/decoder.hpp"
#include "core/register_names.hpp"
#include "gui/style.hpp"

#include <algorithm>
#include <array>
#include <sstream>

namespace ez_arch {

static const std::array<const char*, 12> kOPS = {"add",
                                                 "sub",
                                                 "and",
                                                 "or",
                                                 "slt",
                                                 "addi",
                                                 "andi",
                                                 "ori",
                                                 "lw",
                                                 "sw",
                                                 "beq",
                                                 "bne"};

InstructionBuilderView::InstructionBuilderView(sf::Font& font) : m_font(font) {}

void InstructionBuilderView::setPosition(float x, float y) {
  m_x = x;
  m_y = y;
}
void InstructionBuilderView::setSize(float w, float h) {
  m_w = w;
  m_h = h;
}

void InstructionBuilderView::handleMouseMove(float /*unused*/,
                                             float /*unused*/) {}

static bool within(float x, float y, float rx, float ry, float rw, float rh) {
  return x >= rx && x <= rx + rw && y >= ry && y <= ry + rh;
}

void InstructionBuilderView::handleMousePress(float x, float y) {
  // Detect Add button press
  float bx = m_x + 20;
  float by = m_y + m_h - 50;
  float bw = 100;
  float bh = 30;
  if (within(x, y, bx, by, bw, bh)) { m_addPressed = true; }

  // Dropdown hit regions
  float ddw = 180.F;
  float ddh = 22.F;
  float opx = m_x + 70;
  float opy = m_y + 48;
  float rsx = m_x + 70;
  float rsy = m_y + 78;
  float rtx = m_x + 70;
  float rty = m_y + 108;
  float rdx = m_x + 70;
  float rdy = m_y + 138;
  const int kVIS = 8;

  auto closeAll = [&]() { m_opOpen = m_rsOpen = m_rtOpen = m_rdOpen = false; };

  // 1) If any dropdown is open, prioritize option selection handling first
  bool handled = false;
  if (m_opOpen) {
    for (int vi = 0; vi < kVIS; ++vi) {
      int i = m_opScroll + vi;
      if (i >= static_cast<int>(kOPS.size())) { break; }
      float iy = opy + ddh + (vi * ddh);
      if (within(x, y, opx, iy, ddw, ddh)) {
        m_opIndex = i;
        handled = true;
        break;
      }
    }
  } else if (m_rsOpen) {
    for (int vi = 0; vi < kVIS; ++vi) {
      int i = m_rsScroll + vi;
      if (i >= 32) { break; }
      float iy = rsy + ddh + (vi * ddh);
      if (within(x, y, rsx, iy, ddw, ddh)) {
        m_rs = i;
        handled = true;
        break;
      }
    }
  } else if (m_rtOpen) {
    for (int vi = 0; vi < kVIS; ++vi) {
      int i = m_rtScroll + vi;
      if (i >= 32) { break; }
      float iy = rty + ddh + (vi * ddh);
      if (within(x, y, rtx, iy, ddw, ddh)) {
        m_rt = i;
        handled = true;
        break;
      }
    }
  } else if (m_rdOpen) {
    for (int vi = 0; vi < kVIS; ++vi) {
      int i = m_rdScroll + vi;
      if (i >= 32) { break; }
      float iy = rdy + ddh + (vi * ddh);
      if (within(x, y, rdx, iy, ddw, ddh)) {
        m_rd = i;
        handled = true;
        break;
      }
    }
  }
  if (handled) {
    closeAll();
    return;
  }

  // 2) Toggle open/close for dropdown boxes (now that selection is handled)
  if (within(x, y, opx, opy, ddw, ddh)) {
    m_opOpen = !m_opOpen;
    m_rsOpen = m_rtOpen = m_rdOpen = false;
    return;
  }
  if (within(x, y, rsx, rsy, ddw, ddh)) {
    m_rsOpen = !m_rsOpen;
    m_opOpen = m_rtOpen = m_rdOpen = false;
    return;
  }
  if (within(x, y, rtx, rty, ddw, ddh)) {
    m_rtOpen = !m_rtOpen;
    m_opOpen = m_rsOpen = m_rdOpen = false;
    return;
  }
  if (within(x, y, rdx, rdy, ddw, ddh)) {
    m_rdOpen = !m_rdOpen;
    m_opOpen = m_rsOpen = m_rtOpen = false;
    return;
  }

  // 3) imm box activation
  if (within(x, y, m_x + 70, m_y + 165, 120, 24)) {
    m_immActive = true;
    closeAll();
    return;
  }

  // 4) Click anywhere else closes any open dropdowns
  closeAll();
}

void InstructionBuilderView::handleMouseRelease(float x, float y) {
  float bx = m_x + 20;
  float by = m_y + m_h - 50;
  float bw = 100;
  float bh = 30;
  bool wasPressed = m_addPressed;
  m_addPressed = false;
  if (wasPressed && within(x, y, bx, by, bw, bh)) {
    if (m_onAdd) {
      try {
        std::string asmLine = currentAssembly();
        Decoder::assemble(asmLine); // validate
        m_onAdd(asmLine);
        m_lastError.clear();
      } catch (const std::exception& e) {
        m_lastError = e.what();
      } catch (...) { m_lastError = "Unknown error assembling instruction"; }
    }
  }
}

void InstructionBuilderView::handleMouseWheel(float x, float y, float delta) {
  float ddw = 180.F;
  float ddh = 22.F;
  float opx = m_x + 70;
  float opy = m_y + 48;
  float rsx = m_x + 70;
  float rsy = m_y + 78;
  float rtx = m_x + 70;
  float rty = m_y + 108;
  float rdx = m_x + 70;
  float rdy = m_y + 138;
  const int kVIS = 8;
  auto clampScroll = [&](int& scroll, int size) {
    scroll = std::max(scroll, 0);
    if (scroll > size - kVIS) { scroll = std::max(0, size - kVIS); }
  };
  int dir = (delta > 0) ? -1 : (delta < 0 ? 1 : 0);
  if (dir == 0) { return; }
  if (m_opOpen && within(x, y, opx, opy + ddh, ddw, ddh * kVIS)) {
    m_opScroll += dir;
    clampScroll(m_opScroll, static_cast<int>(kOPS.size()));
  }
  if (m_rsOpen && within(x, y, rsx, rsy + ddh, ddw, ddh * kVIS)) {
    m_rsScroll += dir;
    clampScroll(m_rsScroll, 32);
  }
  if (m_rtOpen && within(x, y, rtx, rty + ddh, ddw, ddh * kVIS)) {
    m_rtScroll += dir;
    clampScroll(m_rtScroll, 32);
  }
  if (m_rdOpen && within(x, y, rdx, rdy + ddh, ddw, ddh * kVIS)) {
    m_rdScroll += dir;
    clampScroll(m_rdScroll, 32);
  }
}

static void drawDropdown(sf::RenderWindow& window,
                         sf::Font& font,
                         float x,
                         float y,
                         float w,
                         float h,
                         int currentIndex,
                         const std::vector<std::string>& options,
                         bool open,
                         int startIndex,
                         int visibleCount) {
  // Main box
  sf::RectangleShape box({w, h});
  box.setPosition({x, y});
  box.setFillColor(sf::Color::White);
  box.setOutlineColor(sf::Color::Black);
  box.setOutlineThickness(1.F);
  window.draw(box);

  sf::Text cur(font);
  std::string current =
      (currentIndex >= 0 && currentIndex < static_cast<int>(options.size()))
          ? options[currentIndex]
          : std::string("");
  cur.setString(current);
  cur.setCharacterSize(14);
  cur.setFillColor(sf::Color::Black);
  cur.setPosition({x + 6, y + 2});
  window.draw(cur);

  if (!open) { return; }

  // Clamp startIndex
  startIndex = std::max(startIndex, 0);
  if (startIndex > static_cast<int>(options.size()) - visibleCount) {
    startIndex = std::max(0, static_cast<int>(options.size()) - visibleCount);
  }

  int endIndex =
      std::min(static_cast<int>(options.size()), startIndex + visibleCount);
  for (int i = startIndex; i < endIndex; ++i) {
    int vi = i - startIndex;
    sf::RectangleShape item({w, h});
    item.setPosition({x, y + h + (static_cast<float>(vi) * h)});
    if (i == currentIndex) {
      item.setFillColor(sf::Color(210, 230, 255));
    } else {
      item.setFillColor(sf::Color(245, 245, 245));
    }
    item.setOutlineColor(sf::Color::Black);
    item.setOutlineThickness(1.F);
    window.draw(item);

    sf::Text t(font);
    t.setString(options[i]);
    t.setCharacterSize(14);
    t.setFillColor(sf::Color::Black);
    t.setPosition({x + 6, y + h + (static_cast<float>(vi) * h) + 2});
    window.draw(t);
  }

  // Optional simple scrollbar indicator
  if (static_cast<int>(options.size()) > visibleCount) {
    float trackH = visibleCount * h;
    float ratio =
        static_cast<float>(visibleCount) / static_cast<float>(options.size());
    float thumbH = trackH * ratio;
    float thumbY = y + h
                   + ((trackH - thumbH)
                      * (static_cast<float>(startIndex)
                         / (options.size() - visibleCount)));
    sf::RectangleShape thumb({4.F, thumbH});
    thumb.setPosition({x + w - 6.F, thumbY});
    thumb.setFillColor(sf::Color(180, 180, 180));
    window.draw(thumb);
  }
}

void InstructionBuilderView::draw(sf::RenderWindow& window) {
  drawPanel(window);

  // Title
  drawLabel(window, "Instruction Builder", m_x + 12, m_y + 12, 18);

  // Dropdowns
  float ddw = 100.F;
  float ddh = 22.F;
  float opx = m_x + 70;
  float opy = m_y + 50;
  float rsx = m_x + 70;
  float rsy = m_y + 80;
  float rtx = m_x + 70;
  float rty = m_y + 110;
  float rdx = m_x + 70;
  float rdy = m_y + 140;
  const int kVIS = 8;

  drawLabel(window, "Op:", m_x + 20, m_y + 50);
  drawLabel(window, "rs:", m_x + 20, m_y + 80);
  drawLabel(window, "rt:", m_x + 20, m_y + 110);
  drawLabel(window, "rd:", m_x + 20, m_y + 140);

  // Immediate textbox
  drawLabel(window, "imm:", m_x + 20, m_y + 170);
  drawTextBox(window, m_imm, m_x + 70, m_y + 170, ddw + 50.F, ddh, m_immActive);

  // draw_dropdown
  std::vector<std::string> regs;
  regs.reserve(32);
  for (int i = 0; i < 32; ++i) {
    regs.emplace_back(kREGISTER_NAMES[i]);
  }
  drawDropdown(window,
               m_font,
               opx,
               opy,
               ddw,
               ddh,
               m_opIndex,
               std::vector<std::string>(kOPS.begin(), kOPS.end()),
               m_opOpen,
               m_opScroll,
               kVIS);
  if (!m_opOpen) {
    drawDropdown(window,
                 m_font,
                 rsx,
                 rsy,
                 ddw,
                 ddh,
                 m_rs,
                 regs,
                 m_rsOpen,
                 m_rsScroll,
                 kVIS);
  }
  if (!m_opOpen && !m_rsOpen) {
    drawDropdown(window,
                 m_font,
                 rtx,
                 rty,
                 ddw,
                 ddh,
                 m_rt,
                 regs,
                 m_rtOpen,
                 m_rtScroll,
                 kVIS);
  }
  if (!m_opOpen && !m_rsOpen && !m_rtOpen) {
    drawDropdown(window,
                 m_font,
                 rdx,
                 rdy,
                 ddw,
                 ddh,
                 m_rd,
                 regs,
                 m_rdOpen,
                 m_rdScroll,
                 kVIS);
  }

  // Live preview and inline validation
  const std::string kPREVIEW = currentAssembly();
  try {
    (void)Decoder::assemble(kPREVIEW);
    m_lastError.clear();
  } catch (const std::exception& e) { m_lastError = e.what(); } catch (...) {
    m_lastError = "Unknown error assembling";
  }

  drawLabel(
      window, std::string("Preview: ") + kPREVIEW, m_x + 250.F, m_y + 100);
  if (!m_lastError.empty()) {
    sf::Text err(m_font);
    err.setString(m_lastError);
    err.setCharacterSize(12);
    err.setFillColor(sf::Color(180, 0, 0));
    err.setPosition({m_x + 20, m_y + 225});
    window.draw(err);
  }

  if (!m_rsOpen && !m_rtOpen && !m_rdOpen && !m_opOpen) {
    // Add button
    drawButton(window, "Add", m_x + 20, m_y + m_h - 40, 50, 30, m_addPressed);
  }
}

void InstructionBuilderView::drawPanel(sf::RenderWindow& window) const {
  sf::RectangleShape bg({m_w, m_h});
  bg.setPosition({m_x, m_y});
  bg.setFillColor(kVIEW_BOX_BACKGROUND_COLOR);
  bg.setOutlineColor(kVIEW_BOX_OUTLINE_COLOR);
  bg.setOutlineThickness(2.F);
  window.draw(bg);
}

void InstructionBuilderView::drawLabel(sf::RenderWindow& window,
                                       const std::string& text,
                                       float x,
                                       float y,
                                       unsigned size) {
  sf::Text t(m_font);
  t.setString(text);
  t.setCharacterSize(size);
  t.setFillColor(sf::Color::Black);
  t.setPosition({x, y});
  window.draw(t);
}

void InstructionBuilderView::drawTextBox(sf::RenderWindow& window,
                                         const std::string& value,
                                         float x,
                                         float y,
                                         float w,
                                         float h,
                                         bool active) {
  sf::RectangleShape box({w, h});
  box.setPosition({x, y});
  box.setFillColor(sf::Color::White);
  box.setOutlineColor(active ? sf::Color::Blue : sf::Color::Black);
  box.setOutlineThickness(1.F);
  window.draw(box);

  sf::Text t(m_font);
  t.setString(value);
  t.setCharacterSize(14);
  t.setFillColor(sf::Color::Black);
  t.setPosition({x + 6, y + 3});
  window.draw(t);
}

void InstructionBuilderView::drawButton(sf::RenderWindow& window,
                                        const std::string& text,
                                        float x,
                                        float y,
                                        float w,
                                        float h,
                                        bool pressed) {
  sf::RectangleShape btn({w, h});
  btn.setPosition({x, y});
  btn.setFillColor(pressed ? kBUTTON_PRESSED_COLOR : kBUTTON_NORMAL_COLOR);
  btn.setOutlineColor(sf::Color::Black);
  btn.setOutlineThickness(1.F);
  window.draw(btn);

  sf::Text t(m_font);
  t.setString(text);
  t.setCharacterSize(14);
  t.setFillColor(kBUTTON_TEXT_COLOR);
  t.setPosition({x + 10, y + 6});
  window.draw(t);
}

std::string InstructionBuilderView::currentAssembly() const {
  std::ostringstream ss;
  const std::string kOP = kOPS[m_opIndex];
  if (kOP == "add" || kOP == "sub" || kOP == "and" || kOP == "or"
      || kOP == "slt") {
    ss << kOP << " " << kREGISTER_NAMES[m_rd] << ", " << kREGISTER_NAMES[m_rs]
       << ", " << kREGISTER_NAMES[m_rt];
  } else if (kOP == "addi" || kOP == "andi" || kOP == "ori") {
    ss << kOP << " " << kREGISTER_NAMES[m_rt] << ", " << kREGISTER_NAMES[m_rs]
       << ", " << (m_imm.empty() ? "0" : m_imm);
  } else if (kOP == "lw" || kOP == "sw") {
    ss << kOP << " " << kREGISTER_NAMES[m_rt] << ", "
       << (m_imm.empty() ? "0" : m_imm) << "(" << kREGISTER_NAMES[m_rs] << ")";
  } else if (kOP == "beq" || kOP == "bne") {
    ss << kOP << " " << kREGISTER_NAMES[m_rs] << ", " << kREGISTER_NAMES[m_rt]
       << ", " << (m_imm.empty() ? "0" : m_imm);
  }
  return ss.str();
}

void InstructionBuilderView::handleTextEntered(uint32_t codepoint) {
  if (!m_immActive) { return; }
  // Accept digits, minus, x/X, and hex letters
  if ((codepoint >= '0' && codepoint <= '9')
      || (codepoint >= 'a' && codepoint <= 'f')
      || (codepoint >= 'A' && codepoint <= 'F') || codepoint == 'x'
      || codepoint == 'X' || codepoint == '-') {
    if (m_imm.size() < 16) { m_imm.push_back(static_cast<char>(codepoint)); }
  }
}

void InstructionBuilderView::handleKeyPressed(int keyCode) {
  // ESC closes dropdowns and unfocus textbox
  if (keyCode == 36 /*Escape*/) {
    m_immActive = false;
    m_opOpen = m_rsOpen = m_rtOpen = m_rdOpen = false;
    return;
  }

  // Up/Down/Enter when any dropdown is open
  auto clampIndex = [](int& idx, int max) {
    idx = std::max(idx, 0);
    if (idx >= max) { idx = max - 1; }
  };
  const int kVIS = 8;
  auto ensureVisible = [&](int sel, int& scroll, int size) {
    scroll = std::min(sel, scroll);
    if (sel >= scroll + kVIS) { scroll = sel - (kVIS - 1); }
    scroll = std::max(scroll, 0);
    if (scroll > size - kVIS) { scroll = std::max(0, size - kVIS); }
  };

  bool anyOpen = m_opOpen || m_rsOpen || m_rtOpen || m_rdOpen;
  if (anyOpen) {
    if (keyCode == 73 /*Up*/) {
      if (m_opOpen) {
        --m_opIndex;
        clampIndex(m_opIndex, static_cast<int>(kOPS.size()));
        ensureVisible(m_opIndex, m_opScroll, static_cast<int>(kOPS.size()));
      } else if (m_rsOpen) {
        --m_rs;
        clampIndex(m_rs, 32);
        ensureVisible(m_rs, m_rsScroll, 32);
      } else if (m_rtOpen) {
        --m_rt;
        clampIndex(m_rt, 32);
        ensureVisible(m_rt, m_rtScroll, 32);
      } else if (m_rdOpen) {
        --m_rd;
        clampIndex(m_rd, 32);
        ensureVisible(m_rd, m_rdScroll, 32);
      }
      return;
    }
    if (keyCode == 74 /*Down*/) {
      if (m_opOpen) {
        ++m_opIndex;
        clampIndex(m_opIndex, static_cast<int>(kOPS.size()));
        ensureVisible(m_opIndex, m_opScroll, static_cast<int>(kOPS.size()));
      } else if (m_rsOpen) {
        ++m_rs;
        clampIndex(m_rs, 32);
        ensureVisible(m_rs, m_rsScroll, 32);
      } else if (m_rtOpen) {
        ++m_rt;
        clampIndex(m_rt, 32);
        ensureVisible(m_rt, m_rtScroll, 32);
      } else if (m_rdOpen) {
        ++m_rd;
        clampIndex(m_rd, 32);
        ensureVisible(m_rd, m_rdScroll, 32);
      }
      return;
    }
    if (keyCode == 58 /*Enter*/) {
      m_opOpen = m_rsOpen = m_rtOpen = m_rdOpen = false;
      return;
    }
  }

  // Backspace in imm field
  if (m_immActive && (keyCode == 59 /*Backspace*/ || keyCode == 8)) {
    if (!m_imm.empty()) { m_imm.pop_back(); }
    return;
  }
}

} // namespace ez_arch
