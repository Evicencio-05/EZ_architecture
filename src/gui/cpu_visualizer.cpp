#include "gui/cpu_visualizer.hpp"

#include <cctype>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "core/cpu.hpp"
#include "core/decoder.hpp"
#include "gui/style.hpp"

namespace ez_arch {

CPUVisualizer::CPUVisualizer(CPU& cpu, sf::RenderWindow& window)
    : m_cpu(cpu), m_window(window) {
  if (!load_font()) {
    std::cerr << "Warning: Could not load font. Text will not display.\n";
  }

  // Initialize view state
  m_activeView = ActiveView::NONE;

  m_needsUpdate = true;

  // Create the register view
  m_registerView =
      std::make_unique<RegisterView>(m_cpu.get_registers(), m_font);
  m_registerView->set_position(kLEFT_SIDEBAR_WIDTH + 20.F, kTOP_BAR_HEIGHT + 20.F);

  // Create the memory view
  m_memoryView = std::make_unique<MemoryView>(m_cpu.get_memory(),
                                              m_cpu.get_registers(), m_font);
  m_memoryView->set_position(kLEFT_SIDEBAR_WIDTH + 20.F, kTOP_BAR_HEIGHT + 20.F);
  m_memoryView->set_display_range(0, 16);

  // Create the instruction view
  m_instructionView = std::make_unique<InstructionView>(
      m_cpu.get_memory(), m_cpu.get_registers(), m_font);
  m_instructionView->set_position(kLEFT_SIDEBAR_WIDTH + 20.F,
                                 kTOP_BAR_HEIGHT + 20.F);
  m_instructionView->set_display_range(0, 16);

  // Create the datapath view for main area
  m_datapathView = std::make_unique<DatapathView>(m_cpu, m_font);

  // Create control buttons - positioned in top bar
  auto step_stage_btn = std::make_unique<Button>("Stage", m_font);
  step_stage_btn->setPosition(700.F, 15.F);
  step_stage_btn->setCallback([this]() {
    m_cpu.step_stage();
    m_needsUpdate = true;
  });
  m_buttons.push_back(std::move(step_stage_btn));

  auto step_btn = std::make_unique<Button>("Step", m_font);
  step_btn->setPosition(800.F, 15.F);
  step_btn->setCallback([this]() {
    m_cpu.step();
    m_needsUpdate = true;
  });
  m_buttons.push_back(std::move(step_btn));

  auto run_btn = std::make_unique<Button>("Run", m_font);
  run_btn->setPosition(900.F, 15.F);
  run_btn->setCallback([this]() {
    m_cpu.run();
    m_needsUpdate = true;
  });
  m_buttons.push_back(std::move(run_btn));

  auto reset_btn = std::make_unique<Button>("Reset", m_font);
  reset_btn->setPosition(1000.F, 15.F);
  reset_btn->setCallback([this]() {
    m_cpu.reset();
    m_needsUpdate = true;
  });
  m_buttons.push_back(std::move(reset_btn));

  // Load queued to memory button
  auto load_btn = std::make_unique<Button>("LoadQ", m_font);
  load_btn->setPosition(1100.F, 15.F);
  load_btn->setCallback([this]() {
    // Append instructions into memory at base address 0x100, sequentially
    const uint32_t kBASE = 0x00000100;
    uint32_t addr = kBASE;
    for (const auto& line : m_instructionQueue) {
      try {
        word_t w = Decoder::assemble(line);
        m_cpu.get_memory().write_word(addr, w);
        addr += 4;
      } catch (...) {
        // ignore malformed lines
      }
    }
    // Set PC to the base of the loaded block
    m_cpu.get_registers().set_pc(kBASE);
    m_needsUpdate = true;
  });
  m_buttons.push_back(std::move(load_btn));

  // Run from queue button
  auto run_q_btn = std::make_unique<Button>("RunQ", m_font);
  run_q_btn->setPosition(1200.F, 15.F);
  run_q_btn->setCallback([this]() {
    const uint32_t kBASE = 0x00000100;
    uint32_t addr = kBASE;
    for (const auto& line : m_instructionQueue) {
      try {
        word_t w = Decoder::assemble(line);
        m_cpu.get_memory().write_word(addr, w);
        addr += 4;
      } catch (...) {
      }
    }
    m_cpu.get_registers().set_pc(kBASE);
    m_cpu.run();
    m_needsUpdate = true;
  });
  m_buttons.push_back(std::move(run_q_btn));

  // Create toggle buttons - positioned in left sidebar
  auto reg_toggle = std::make_unique<Button>("R", m_font);
  reg_toggle->setPosition(5.F, TOP_BAR_HEIGHT + 10.F);
  reg_toggle->setSize(TOGGLE_BUTTON_SIZE, TOGGLE_BUTTON_SIZE);
  reg_toggle->setCallback([this]() {
    m_activeView = (m_activeView == ActiveView::REGISTERS)
                       ? ActiveView::NONE
                       : ActiveView::REGISTERS;
    m_needsUpdate = true;
  });
  m_buttons.push_back(std::move(reg_toggle));

  auto mem_toggle = std::make_unique<Button>("M", m_font);
  mem_toggle->setPosition(5.F, TOP_BAR_HEIGHT + 70.F);
  mem_toggle->setSize(TOGGLE_BUTTON_SIZE, TOGGLE_BUTTON_SIZE);
  mem_toggle->setCallback([this]() {
    m_activeView = (m_activeView == ActiveView::MEMORY) ? ActiveView::NONE
                                                        : ActiveView::MEMORY;
    m_needsUpdate = true;
  });
  m_buttons.push_back(std::move(mem_toggle));

  auto inst_toggle = std::make_unique<Button>("I", m_font);
  inst_toggle->setPosition(5.F, TOP_BAR_HEIGHT + 130.F);
  inst_toggle->setSize(TOGGLE_BUTTON_SIZE, TOGGLE_BUTTON_SIZE);
  inst_toggle->setCallback([this]() {
    m_activeView = (m_activeView == ActiveView::INSTRUCTIONS)
                       ? ActiveView::NONE
                       : ActiveView::INSTRUCTIONS;
    m_needsUpdate = true;
  });
  m_buttons.push_back(std::move(inst_toggle));

  // Instruction Builder toggle
  auto builder_toggle = std::make_unique<Button>("B", m_font);
  builder_toggle->setPosition(5.F, TOP_BAR_HEIGHT + 190.F);
  builder_toggle->setSize(TOGGLE_BUTTON_SIZE, TOGGLE_BUTTON_SIZE);
  builder_toggle->setCallback([this]() {
    m_activeView = (m_activeView == ActiveView::BUILDER) ? ActiveView::NONE
                                                         : ActiveView::BUILDER;
    m_needsUpdate = true;
  });
  m_buttons.push_back(std::move(builder_toggle));

  // Instruction Queue toggle
  auto queue_toggle = std::make_unique<Button>("Q", m_font);
  queue_toggle->setPosition(5.F, TOP_BAR_HEIGHT + 250.F);
  queue_toggle->setSize(TOGGLE_BUTTON_SIZE, TOGGLE_BUTTON_SIZE);
  queue_toggle->setCallback([this]() {
    m_activeView = (m_activeView == ActiveView::QUEUE) ? ActiveView::NONE
                                                       : ActiveView::QUEUE;
    m_needsUpdate = true;
  });
  m_buttons.push_back(std::move(queue_toggle));

  // Builder and Queue views
  m_builderView = std::make_unique<InstructionBuilderView>(m_font);
  m_queueView = std::make_unique<InstructionQueueView>(m_font);

  // Load cached instructions
  m_instructionQueue = InstructionCache::load();
  m_queueView->set_items(m_instructionQueue);

  // Position views
  m_builderView->set_position(LEFT_SIDEBAR_WIDTH + 20.F, TOP_BAR_HEIGHT + 20.F);
  m_builderView->set_size(500.F, 240.F);
  m_queueView->set_position(LEFT_SIDEBAR_WIDTH + 540.F, TOP_BAR_HEIGHT + 20.F);
  m_queueView->set_size(500.F, 300.F);

  // Wire callbacks
  m_builderView->set_on_add([this](const std::string& line) {
    m_instructionQueue.push_back(line);
    InstructionCache::append(line);
    m_queueView->set_items(m_instructionQueue);
  });

  m_queueView->set_on_delete([this](size_t idx) {
    if (idx < m_instructionQueue.size()) {
      m_instructionQueue.erase(m_instructionQueue.begin() + idx);
      sync_queue_to_cache();
      m_queueView->set_items(m_instructionQueue);
    }
  });

  m_queueView->set_on_clear([this]() {
    m_instructionQueue.clear();
    InstructionCache::clear();
    m_queueView->set_items(m_instructionQueue);
  });

  // Reorder callbacks
  m_queueView->set_on_move([this](size_t idx, int delta) {
    if (idx < m_instructionQueue.size()) {
      auto new_idx = static_cast<size_t>(static_cast<int>(idx) + delta);
      if (new_idx < m_instructionQueue.size()) {
        std::swap(m_instructionQueue[idx], m_instructionQueue[new_idx]);
        sync_queue_to_cache();
        m_queueView->set_items(m_instructionQueue);
      }
    }
  });
  auto initial_size = m_window.getSize();
  handle_resize(initial_size.x, initial_size.y);
}

bool CPUVisualizer::load_font() {
  const char* font_paths[] = {
      "/usr/share/fonts/TTF/DejaVuSans.ttf",
      "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
      "/usr/share/fonts/liberation/LiberationSans-Regular.ttf",
      "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
      R"(C:\Windows\Fonts\arial.ttf)",};

  for (const char* path : font_paths) {
    if (m_font.openFromFile(path)) {
      std::cout << "Loaded font from: " << path << "\n";
      if (!m_font.isSmooth()) {
        m_font.setSmooth(true);
      }
      return true;
    }
  }

  std::cerr << "ERROR: Could not load any font!\n";
  return false;
}

void CPUVisualizer::handle_resize(unsigned width, unsigned height) {
  m_datapathView->set_position(LEFT_SIDEBAR_WIDTH, TOP_BAR_HEIGHT);
  m_datapathView->set_size(width - LEFT_SIDEBAR_WIDTH, height - TOP_BAR_HEIGHT);

  // TODO(evice): Resize other views

  m_needsUpdate = true;
}

void CPUVisualizer::update() {
  // Update all view components
  if (m_needsUpdate) {
    m_registerView->update();
    m_memoryView->update();
    m_instructionView->update();
    m_datapathView->update();
    m_needsUpdate = false;
  }
}

void CPUVisualizer::draw() {
  // Draw layout sections
  draw_top_bar();
  draw_left_sidebar();
  draw_main_area();
  draw_active_view();

  // Draw buttons
  for (auto& btn : m_buttons) {
    btn->draw(m_window);
  }
}

void CPUVisualizer::draw_pipeline_stage() {
  // Compact version for top bar
  ExecutionStage stage = m_cpu.get_current_stage();
  std::string stage_text = std::string(stage_to_string(stage));

  sf::Text stage_display(m_font);
  stage_display.setString(stage_text);
  stage_display.setCharacterSize(16);
  stage_display.setFillColor(TITLE_TEXT_COLOR);
  stage_display.setPosition({20.F, 20.F});
  m_window.draw(stage_display);
}

void CPUVisualizer::handle_mouse_move(float x, float y) {
  for (auto& btn : m_buttons) {
    btn->handleMouseMove(x, y);
  }
  if (m_activeView == ActiveView::BUILDER && m_builderView) {
    m_builderView->handle_mouse_move(x, y);
}
  if (m_activeView == ActiveView::QUEUE && m_queueView) {
    m_queueView->handle_mouse_move(x, y);
}
}

void CPUVisualizer::handle_mouse_press(float x, float y) {
  for (auto& btn : m_buttons) {
    btn->handleMousePress(x, y);
  }
  if (m_activeView == ActiveView::BUILDER && m_builderView) {
    m_builderView->handle_mouse_press(x, y);
}
  if (m_activeView == ActiveView::QUEUE && m_queueView) {
    m_queueView->handle_mouse_press(x, y);
}
}

void CPUVisualizer::handle_mouse_release(float x, float y) {
  for (auto& btn : m_buttons) {
    btn->handleMouseRelease(x, y);
  }
  if (m_activeView == ActiveView::BUILDER && m_builderView) {
    m_builderView->handle_mouse_release(x, y);
}
  if (m_activeView == ActiveView::QUEUE && m_queueView) {
    m_queueView->handle_mouse_release(x, y);
}
}

void CPUVisualizer::handle_mouse_wheel(float x, float y, float delta) {
  if (m_activeView == ActiveView::BUILDER && m_builderView) {
    m_builderView->handle_mouse_wheel(x, y, delta);
}
}

void CPUVisualizer::draw_top_bar() {
  auto window_size = m_window.getSize();

  // Draw top bar background
  sf::RectangleShape top_bar({static_cast<float>(window_size.x), TOP_BAR_HEIGHT});
  top_bar.setPosition({0, 0});
  top_bar.setFillColor(VIEW_BOX_BACKGROUND_COLOR);
  top_bar.setOutlineColor(VIEW_BOX_OUTLINE_COLOR);
  top_bar.setOutlineThickness(2.F);
  m_window.draw(top_bar);

  // Draw pipeline stage (compact version)
  draw_pipeline_stage();

  // Draw PC info
  sf::Text pc_text(m_font);
  std::ostringstream pc_stream;
  pc_stream << "PC: 0x" << std::hex << std::setw(8) << std::setfill('0')
           << m_cpu.get_registers().get_pc();
  pc_text.setString(pc_stream.str());
  pc_text.setCharacterSize(16);
  pc_text.setFillColor(TITLE_TEXT_COLOR);
  pc_text.setPosition({300.F, 20.F});
  m_window.draw(pc_text);

  // Draw current instruction
  word_t pc = m_cpu.get_registers().get_pc();
  word_t instr = m_cpu.get_memory().read_word(pc);
  std::string decoded = Decoder::decode(instr);

  sf::Text instr_text(m_font);
  instr_text.setString("Instruction: " + decoded);
  instr_text.setCharacterSize(16);
  instr_text.setFillColor(TITLE_TEXT_COLOR);
  instr_text.setPosition({500.F, 20.F});
  m_window.draw(instr_text);
}

void CPUVisualizer::draw_left_sidebar() {
  auto window_size = m_window.getSize();

  // Draw sidebar background
  sf::RectangleShape sidebar(
      {LEFT_SIDEBAR_WIDTH, static_cast<float>(window_size.y - TOP_BAR_HEIGHT)});
  sidebar.setPosition({0, TOP_BAR_HEIGHT});
  sidebar.setFillColor(VIEW_BOX_BACKGROUND_COLOR);
  m_window.draw(sidebar);
}

void CPUVisualizer::draw_main_area() {
  auto window_size = m_window.getSize();

  // Draw main area background
  sf::RectangleShape main_area(
      {static_cast<float>(window_size.x - LEFT_SIDEBAR_WIDTH),
       static_cast<float>(window_size.y - TOP_BAR_HEIGHT)});
  main_area.setPosition({LEFT_SIDEBAR_WIDTH, TOP_BAR_HEIGHT});
  main_area.setFillColor(DATA_PATH_BACKGROUND_COLOR);
  m_window.draw(main_area);

  // Draw the datapath diagram
  m_datapathView->draw(m_window);
  // m_datapathView->drawScaffolding(m_window, mainArea.getSize());
}

void CPUVisualizer::draw_active_view() {
  // Draw the selected view on top of the main area
  switch (m_activeView) {
    case ActiveView::REGISTERS:
      m_registerView->draw(m_window);
      break;
    case ActiveView::MEMORY:
      m_memoryView->draw(m_window);
      break;
    case ActiveView::INSTRUCTIONS:
      m_instructionView->draw(m_window);
      break;
    case ActiveView::BUILDER:
      if (m_builderView) { m_builderView->draw(m_window);
}
      if (m_queueView) { m_queueView->draw(m_window);
}
      break;
    case ActiveView::QUEUE:
      if (m_queueView) { m_queueView->draw(m_window);
}
      break;
    case ActiveView::NONE:
      // Nothing to draw
      break;
  }
}

void CPUVisualizer::handle_text_entered(uint32_t codepoint) {
  bool builder_active =
      (m_activeView == ActiveView::BUILDER && m_builderView != nullptr);
  if (builder_active) {
    m_builderView->handle_text_entered(codepoint);
  }

  // Global hotkeys via text input (only for printable characters)
  char ch = static_cast<char>(codepoint);
  if (std::isprint(static_cast<unsigned char>(ch)) != 0) {
    char c = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));

    // If builder is active and not typing and no dropdown open, allow
    // builder-specific hotkeys
    if (builder_active && !m_builderView->text_active() &&
        !m_builderView->any_dropdown_open()) {
      if (c == 'o') {
        m_builderView->open_op();
        return;
      }
      if (c == '1') {
        m_builderView->open_rs();
        return;
      }
      if (c == '2') {
        m_builderView->open_rt();
        return;
      }
      if (c == '3') {
        m_builderView->open_rd();
        return;
      }
    }

    // View toggles (work regardless of active view unless typing text in
    // builder)
    if (!builder_active ||
        (!m_builderView->text_active() && !m_builderView->any_dropdown_open())) {
      if (c == 'r') {
        m_activeView = (m_activeView == ActiveView::REGISTERS)
                           ? ActiveView::NONE
                           : ActiveView::REGISTERS;
        return;
      }
      if (c == 'm') {
        m_activeView = (m_activeView == ActiveView::MEMORY)
                           ? ActiveView::NONE
                           : ActiveView::MEMORY;
        return;
      }
      if (c == 'i') {
        m_activeView = (m_activeView == ActiveView::INSTRUCTIONS)
                           ? ActiveView::NONE
                           : ActiveView::INSTRUCTIONS;
        return;
      }
      if (c == 'b') {
        m_activeView = (m_activeView == ActiveView::BUILDER)
                           ? ActiveView::NONE
                           : ActiveView::BUILDER;
        return;
      }
      if (c == 'q') {
        m_activeView = (m_activeView == ActiveView::QUEUE) ? ActiveView::NONE
                                                           : ActiveView::QUEUE;
        return;
      }
    }
  }
}

void CPUVisualizer::handle_key_pressed(int key_code) {
  if (m_activeView == ActiveView::BUILDER && m_builderView) {
    m_builderView->handle_key_pressed(key_code);
  }
}

void CPUVisualizer::sync_queue_to_cache() {
  InstructionCache::save(m_instructionQueue);
}

}  // namespace ez_arch
