#include "gui/cpu_visualizer.hpp"

#include <cctype>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "core/decoder.hpp"
#include "gui/style.hpp"

namespace ez_arch {

CPUVisualizer::CPUVisualizer(CPU& cpu, sf::RenderWindow& window)
    : m_cpu(cpu), m_window(window) {
  if (!loadFont()) {
    std::cerr << "Warning: Could not load font. Text will not display.\n";
  }

  // Initialize view state
  m_activeView = ActiveView::NONE;

  m_needsUpdate = true;

  // Create the register view
  m_registerView =
      std::make_unique<RegisterView>(m_cpu.get_registers(), m_font);
  m_registerView->setPosition(LEFT_SIDEBAR_WIDTH + 20.f, TOP_BAR_HEIGHT + 20.f);

  // Create the memory view
  m_memoryView = std::make_unique<MemoryView>(m_cpu.get_memory(),
                                              m_cpu.get_registers(), m_font);
  m_memoryView->setPosition(LEFT_SIDEBAR_WIDTH + 20.f, TOP_BAR_HEIGHT + 20.f);
  m_memoryView->setDisplayRange(0, 16);

  // Create the instruction view
  m_instructionView = std::make_unique<InstructionView>(
      m_cpu.get_memory(), m_cpu.get_registers(), m_font);
  m_instructionView->setPosition(LEFT_SIDEBAR_WIDTH + 20.f,
                                 TOP_BAR_HEIGHT + 20.f);
  m_instructionView->setDisplayRange(0, 16);

  // Create the datapath view for main area
  m_datapathView = std::make_unique<DatapathView>(m_cpu, m_font);

  // Create control buttons - positioned in top bar
  auto stepStageBtn = std::make_unique<Button>("Stage", m_font);
  stepStageBtn->setPosition(700.f, 15.f);
  stepStageBtn->setCallback([this]() {
    m_cpu.step_stage();
    m_needsUpdate = true;
  });
  m_buttons.push_back(std::move(stepStageBtn));

  auto stepBtn = std::make_unique<Button>("Step", m_font);
  stepBtn->setPosition(800.f, 15.f);
  stepBtn->setCallback([this]() {
    m_cpu.step();
    m_needsUpdate = true;
  });
  m_buttons.push_back(std::move(stepBtn));

  auto runBtn = std::make_unique<Button>("Run", m_font);
  runBtn->setPosition(900.f, 15.f);
  runBtn->setCallback([this]() {
    m_cpu.run();
    m_needsUpdate = true;
  });
  m_buttons.push_back(std::move(runBtn));

  auto resetBtn = std::make_unique<Button>("Reset", m_font);
  resetBtn->setPosition(1000.f, 15.f);
  resetBtn->setCallback([this]() {
    m_cpu.reset();
    m_needsUpdate = true;
  });
  m_buttons.push_back(std::move(resetBtn));

  // Load queued to memory button
  auto loadBtn = std::make_unique<Button>("LoadQ", m_font);
  loadBtn->setPosition(1100.f, 15.f);
  loadBtn->setCallback([this]() {
    // Append instructions into memory at base address 0x100, sequentially
    const uint32_t base = 0x00000100;
    uint32_t addr = base;
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
    m_cpu.get_registers().set_pc(base);
    m_needsUpdate = true;
  });
  m_buttons.push_back(std::move(loadBtn));

  // Run from queue button
  auto runQBtn = std::make_unique<Button>("RunQ", m_font);
  runQBtn->setPosition(1200.f, 15.f);
  runQBtn->setCallback([this]() {
    const uint32_t base = 0x00000100;
    uint32_t addr = base;
    for (const auto& line : m_instructionQueue) {
      try {
        word_t w = Decoder::assemble(line);
        m_cpu.get_memory().write_word(addr, w);
        addr += 4;
      } catch (...) {
      }
    }
    m_cpu.get_registers().set_pc(base);
    m_cpu.run();
    m_needsUpdate = true;
  });
  m_buttons.push_back(std::move(runQBtn));

  // Create toggle buttons - positioned in left sidebar
  auto regToggle = std::make_unique<Button>("R", m_font);
  regToggle->setPosition(5.f, TOP_BAR_HEIGHT + 10.f);
  regToggle->setSize(TOGGLE_BUTTON_SIZE, TOGGLE_BUTTON_SIZE);
  regToggle->setCallback([this]() {
    m_activeView = (m_activeView == ActiveView::REGISTERS)
                       ? ActiveView::NONE
                       : ActiveView::REGISTERS;
    m_needsUpdate = true;
  });
  m_buttons.push_back(std::move(regToggle));

  auto memToggle = std::make_unique<Button>("M", m_font);
  memToggle->setPosition(5.f, TOP_BAR_HEIGHT + 70.f);
  memToggle->setSize(TOGGLE_BUTTON_SIZE, TOGGLE_BUTTON_SIZE);
  memToggle->setCallback([this]() {
    m_activeView = (m_activeView == ActiveView::MEMORY) ? ActiveView::NONE
                                                        : ActiveView::MEMORY;
    m_needsUpdate = true;
  });
  m_buttons.push_back(std::move(memToggle));

  auto instToggle = std::make_unique<Button>("I", m_font);
  instToggle->setPosition(5.f, TOP_BAR_HEIGHT + 130.f);
  instToggle->setSize(TOGGLE_BUTTON_SIZE, TOGGLE_BUTTON_SIZE);
  instToggle->setCallback([this]() {
    m_activeView = (m_activeView == ActiveView::INSTRUCTIONS)
                       ? ActiveView::NONE
                       : ActiveView::INSTRUCTIONS;
    m_needsUpdate = true;
  });
  m_buttons.push_back(std::move(instToggle));

  // Instruction Builder toggle
  auto builderToggle = std::make_unique<Button>("B", m_font);
  builderToggle->setPosition(5.f, TOP_BAR_HEIGHT + 190.f);
  builderToggle->setSize(TOGGLE_BUTTON_SIZE, TOGGLE_BUTTON_SIZE);
  builderToggle->setCallback([this]() {
    m_activeView = (m_activeView == ActiveView::BUILDER) ? ActiveView::NONE
                                                         : ActiveView::BUILDER;
    m_needsUpdate = true;
  });
  m_buttons.push_back(std::move(builderToggle));

  // Instruction Queue toggle
  auto queueToggle = std::make_unique<Button>("Q", m_font);
  queueToggle->setPosition(5.f, TOP_BAR_HEIGHT + 250.f);
  queueToggle->setSize(TOGGLE_BUTTON_SIZE, TOGGLE_BUTTON_SIZE);
  queueToggle->setCallback([this]() {
    m_activeView = (m_activeView == ActiveView::QUEUE) ? ActiveView::NONE
                                                       : ActiveView::QUEUE;
    m_needsUpdate = true;
  });
  m_buttons.push_back(std::move(queueToggle));

  // Builder and Queue views
  m_builderView = std::make_unique<InstructionBuilderView>(m_font);
  m_queueView = std::make_unique<InstructionQueueView>(m_font);

  // Load cached instructions
  m_instructionQueue = InstructionCache::load();
  m_queueView->setItems(m_instructionQueue);

  // Position views
  m_builderView->setPosition(LEFT_SIDEBAR_WIDTH + 20.f, TOP_BAR_HEIGHT + 20.f);
  m_builderView->setSize(500.f, 240.f);
  m_queueView->setPosition(LEFT_SIDEBAR_WIDTH + 540.f, TOP_BAR_HEIGHT + 20.f);
  m_queueView->setSize(500.f, 300.f);

  // Wire callbacks
  m_builderView->setOnAdd([this](const std::string& line) {
    m_instructionQueue.push_back(line);
    InstructionCache::append(line);
    m_queueView->setItems(m_instructionQueue);
  });

  m_queueView->setOnDelete([this](size_t idx) {
    if (idx < m_instructionQueue.size()) {
      m_instructionQueue.erase(m_instructionQueue.begin() + idx);
      syncQueueToCache();
      m_queueView->setItems(m_instructionQueue);
    }
  });

  m_queueView->setOnClear([this]() {
    m_instructionQueue.clear();
    InstructionCache::clear();
    m_queueView->setItems(m_instructionQueue);
  });

  // Reorder callbacks
  m_queueView->setOnMove([this](size_t idx, int delta) {
    if (idx < m_instructionQueue.size()) {
      size_t newIdx = static_cast<size_t>(static_cast<int>(idx) + delta);
      if (newIdx < m_instructionQueue.size()) {
        std::swap(m_instructionQueue[idx], m_instructionQueue[newIdx]);
        syncQueueToCache();
        m_queueView->setItems(m_instructionQueue);
      }
    }
  });
  auto initialSize = m_window.getSize();
  handleResize(initialSize.x, initialSize.y);
}

bool CPUVisualizer::loadFont() {
  const char* fontPaths[] = {
      // "/usr/share/fonts/TTF/DejaVuSans.ttf",
      // "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
      "/usr/share/fonts/liberation/LiberationSans-Regular.ttf",
      "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
      "/home/evice/.local/share/fonts/omarchy.ttf"};

  for (const char* path : fontPaths) {
    if (m_font.openFromFile(path)) {
      std::cout << "Loaded font from: " << path << "\n";
      return true;
    }
  }

  std::cerr << "ERROR: Could not load any font!\n";
  return false;
}

void CPUVisualizer::handleResize(unsigned width, unsigned height) {
  m_datapathView->setPosition(LEFT_SIDEBAR_WIDTH, TOP_BAR_HEIGHT);
  m_datapathView->setSize(width - LEFT_SIDEBAR_WIDTH, height - TOP_BAR_HEIGHT);

  // TODO: Resize other views

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
  drawTopBar();
  drawLeftSidebar();
  drawMainArea();
  drawActiveView();

  // Draw buttons
  for (auto& btn : m_buttons) {
    btn->draw(m_window);
  }
}

void CPUVisualizer::drawPipelineStage() {
  // Compact version for top bar
  ExecutionStage stage = m_cpu.get_current_stage();
  std::string stageText = std::string(stageToString(stage));

  sf::Text stageDisplay(m_font);
  stageDisplay.setString(stageText);
  stageDisplay.setCharacterSize(16);
  stageDisplay.setFillColor(TITLE_TEXT_COLOR);
  stageDisplay.setPosition({20.f, 20.f});
  m_window.draw(stageDisplay);
}

void CPUVisualizer::handleMouseMove(float x, float y) {
  for (auto& btn : m_buttons) {
    btn->handleMouseMove(x, y);
  }
  if (m_activeView == ActiveView::BUILDER && m_builderView)
    m_builderView->handleMouseMove(x, y);
  if (m_activeView == ActiveView::QUEUE && m_queueView)
    m_queueView->handleMouseMove(x, y);
}

void CPUVisualizer::handleMousePress(float x, float y) {
  for (auto& btn : m_buttons) {
    btn->handleMousePress(x, y);
  }
  if (m_activeView == ActiveView::BUILDER && m_builderView)
    m_builderView->handleMousePress(x, y);
  if (m_activeView == ActiveView::QUEUE && m_queueView)
    m_queueView->handleMousePress(x, y);
}

void CPUVisualizer::handleMouseRelease(float x, float y) {
  for (auto& btn : m_buttons) {
    btn->handleMouseRelease(x, y);
  }
  if (m_activeView == ActiveView::BUILDER && m_builderView)
    m_builderView->handleMouseRelease(x, y);
  if (m_activeView == ActiveView::QUEUE && m_queueView)
    m_queueView->handleMouseRelease(x, y);
}

void CPUVisualizer::handleMouseWheel(float x, float y, float delta) {
  if (m_activeView == ActiveView::BUILDER && m_builderView)
    m_builderView->handleMouseWheel(x, y, delta);
}

void CPUVisualizer::drawTopBar() {
  auto windowSize = m_window.getSize();

  // Draw top bar background
  sf::RectangleShape topBar({static_cast<float>(windowSize.x), TOP_BAR_HEIGHT});
  topBar.setPosition({0, 0});
  topBar.setFillColor(VIEW_BOX_BACKGROUND_COLOR);
  topBar.setOutlineColor(VIEW_BOX_OUTLINE_COLOR);
  topBar.setOutlineThickness(2.f);
  m_window.draw(topBar);

  // Draw pipeline stage (compact version)
  drawPipelineStage();

  // Draw PC info
  sf::Text pcText(m_font);
  std::ostringstream pcStream;
  pcStream << "PC: 0x" << std::hex << std::setw(8) << std::setfill('0')
           << m_cpu.get_registers().get_pc();
  pcText.setString(pcStream.str());
  pcText.setCharacterSize(16);
  pcText.setFillColor(TITLE_TEXT_COLOR);
  pcText.setPosition({300.f, 20.f});
  m_window.draw(pcText);

  // Draw current instruction
  word_t pc = m_cpu.get_registers().get_pc();
  word_t instr = m_cpu.get_memory().read_word(pc);
  std::string decoded = Decoder::decode(instr);

  sf::Text instrText(m_font);
  instrText.setString("Instruction: " + decoded);
  instrText.setCharacterSize(16);
  instrText.setFillColor(TITLE_TEXT_COLOR);
  instrText.setPosition({500.f, 20.f});
  m_window.draw(instrText);
}

void CPUVisualizer::drawLeftSidebar() {
  auto windowSize = m_window.getSize();

  // Draw sidebar background
  sf::RectangleShape sidebar(
      {LEFT_SIDEBAR_WIDTH, static_cast<float>(windowSize.y - TOP_BAR_HEIGHT)});
  sidebar.setPosition({0, TOP_BAR_HEIGHT});
  sidebar.setFillColor(VIEW_BOX_BACKGROUND_COLOR);
  m_window.draw(sidebar);
}

void CPUVisualizer::drawMainArea() {
  auto windowSize = m_window.getSize();

  // Draw main area background
  sf::RectangleShape mainArea(
      {static_cast<float>(windowSize.x - LEFT_SIDEBAR_WIDTH),
       static_cast<float>(windowSize.y - TOP_BAR_HEIGHT)});
  mainArea.setPosition({LEFT_SIDEBAR_WIDTH, TOP_BAR_HEIGHT});
  mainArea.setFillColor(DATA_PATH_BACKGROUND_COLOR);
  m_window.draw(mainArea);

  // Draw the datapath diagram
  m_datapathView->draw(m_window);
  m_datapathView->drawScaffolding(m_window, mainArea.getSize());
}

void CPUVisualizer::drawActiveView() {
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
      if (m_builderView) m_builderView->draw(m_window);
      if (m_queueView) m_queueView->draw(m_window);
      break;
    case ActiveView::QUEUE:
      if (m_queueView) m_queueView->draw(m_window);
      break;
    case ActiveView::NONE:
      // Nothing to draw
      break;
  }
}

void CPUVisualizer::handleTextEntered(uint32_t codepoint) {
  bool builderActive =
      (m_activeView == ActiveView::BUILDER && m_builderView != nullptr);
  if (builderActive) {
    m_builderView->handleTextEntered(codepoint);
  }

  // Global hotkeys via text input (only for printable characters)
  char ch = static_cast<char>(codepoint);
  if (std::isprint(static_cast<unsigned char>(ch))) {
    char c = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));

    // If builder is active and not typing and no dropdown open, allow
    // builder-specific hotkeys
    if (builderActive && !m_builderView->textActive() &&
        !m_builderView->anyDropdownOpen()) {
      if (c == 'o') {
        m_builderView->openOp();
        return;
      }
      if (c == '1') {
        m_builderView->openRS();
        return;
      }
      if (c == '2') {
        m_builderView->openRT();
        return;
      }
      if (c == '3') {
        m_builderView->openRD();
        return;
      }
    }

    // View toggles (work regardless of active view unless typing text in
    // builder)
    if (!builderActive ||
        (!m_builderView->textActive() && !m_builderView->anyDropdownOpen())) {
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

void CPUVisualizer::handleKeyPressed(int keyCode) {
  if (m_activeView == ActiveView::BUILDER && m_builderView) {
    m_builderView->handleKeyPressed(keyCode);
  }
}

void CPUVisualizer::syncQueueToCache() {
  InstructionCache::save(m_instructionQueue);
}

}  // namespace ez_arch
