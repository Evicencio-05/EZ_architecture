#include "gui/cpu_visualizer.hpp"

#include "core/cpu.hpp"
#include "core/decoder.hpp"
#include "gui/instruction_cache.hpp"
#include "gui/style.hpp"

#include <array>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string_view>

namespace ez_arch {

CPUVisualizer::CPUVisualizer(CPU& cpu, sf::RenderWindow& window)
    : m_cpu(cpu), m_window(window) {
  if (!loadFont(m_font)) {
    std::cerr << "Warning: Could not load font. Text will not display.\n";
  }

  m_activeView = ActiveView::NONE;

  m_needsUpdate = true;
  m_updateTopBar = true;
  m_runningInstructions = false;

  m_registerView = std::make_unique<RegisterView>(m_cpu.getRegisters(), m_font);
  m_registerView->setPosition(kLEFT_SIDEBAR_WIDTH + 20.F,
                              kTOP_BAR_HEIGHT + 20.F);

  m_memoryView = std::make_unique<MemoryView>(
      m_cpu.getMemory(), m_cpu.getRegisters(), m_font);
  m_memoryView->setPosition(kLEFT_SIDEBAR_WIDTH + 20.F, kTOP_BAR_HEIGHT + 20.F);
  m_memoryView->setDisplayRange(0, 16);

  m_instructionView = std::make_unique<InstructionView>(
      m_cpu.getMemory(), m_cpu.getRegisters(), m_font);
  m_instructionView->setPosition(kLEFT_SIDEBAR_WIDTH + 20.F,
                                 kTOP_BAR_HEIGHT + 20.F);
  m_instructionView->setDisplayRange(0, 16);

  m_datapathView = std::make_unique<DatapathView>(m_cpu, m_font);

  auto stepStageBtn = std::make_unique<Button>("Stage", m_font);
  stepStageBtn->setPosition(700.F, 15.F);
  stepStageBtn->setCallback([this]() {
    m_cpu.stepStage();
    m_needsUpdate = true;
    m_updateTopBar = true;
  });
  m_buttons.push_back(std::move(stepStageBtn));

  auto stepBtn = std::make_unique<Button>("Step", m_font);
  stepBtn->setPosition(800.F, 15.F);
  stepBtn->setCallback([this]() {
    m_cpu.step();
    m_needsUpdate = true;
    m_updateTopBar = true;
  });
  m_buttons.push_back(std::move(stepBtn));

  auto runBtn = std::make_unique<Button>("Run", m_font);
  runBtn->setPosition(900.F, 15.F);
  runBtn->setCallback([this]() {
    m_cpu.run();
    m_needsUpdate = true;
    m_runningInstructions = true;
  });
  m_buttons.push_back(std::move(runBtn));

  auto resetBtn = std::make_unique<Button>("Reset", m_font);
  resetBtn->setPosition(1000.F, 15.F);
  resetBtn->setCallback([this]() {
    m_cpu.reset();
    m_needsUpdate = true;
    m_updateTopBar = true;
  });
  m_buttons.push_back(std::move(resetBtn));

  auto loadBtn = std::make_unique<Button>("LoadQ", m_font);
  loadBtn->setPosition(1100.F, 15.F);
  loadBtn->setCallback([this]() {
    // Append instructions into memory at base address 0x100
    const uint32_t kBASE = 0x00000100;
    uint32_t addr = kBASE;
    for (const auto& line : m_instructionQueue) {
      try {
        word_t w = Decoder::assemble(line);
        m_cpu.getMemory().writeWord(addr, w);
        addr += 4;
      } catch (...) {
        // ignore malformed lines
      }
    }
    // Set PC to the base of the loaded block
    m_cpu.getRegisters().setPc(kBASE);
    m_needsUpdate = true;
    m_updateTopBar = true;
  });
  m_buttons.push_back(std::move(loadBtn));

  auto runQBtn = std::make_unique<Button>("RunQ", m_font);
  runQBtn->setPosition(1200.F, 15.F);
  runQBtn->setCallback([this]() {
    const uint32_t kBASE = 0x00000100;
    uint32_t addr = kBASE;
    for (const auto& line : m_instructionQueue) {
      try {
        word_t w = Decoder::assemble(line);
        m_cpu.getMemory().writeWord(addr, w);
        addr += 4;
      } catch (...) {}
    }
    m_cpu.getRegisters().setPc(kBASE);
    m_cpu.run();
    m_needsUpdate = true;
    m_runningInstructions = true;
  });
  m_buttons.push_back(std::move(runQBtn));

  auto regToggle = std::make_unique<Button>("R", m_font);
  regToggle->setPosition(5.F, kTOP_BAR_HEIGHT + 10.F);
  regToggle->setSize(kTOGGLE_BUTTON_SIZE, kTOGGLE_BUTTON_SIZE);
  regToggle->setCallback([this]() {
    m_activeView = (m_activeView == ActiveView::REGISTERS)
                       ? ActiveView::NONE
                       : ActiveView::REGISTERS;
    m_needsUpdate = true;
  });
  m_buttons.push_back(std::move(regToggle));

  auto memToggle = std::make_unique<Button>("M", m_font);
  memToggle->setPosition(5.F, kTOP_BAR_HEIGHT + 70.F);
  memToggle->setSize(kTOGGLE_BUTTON_SIZE, kTOGGLE_BUTTON_SIZE);
  memToggle->setCallback([this]() {
    m_activeView = (m_activeView == ActiveView::MEMORY) ? ActiveView::NONE
                                                        : ActiveView::MEMORY;
    m_needsUpdate = true;
  });
  m_buttons.push_back(std::move(memToggle));

  auto instToggle = std::make_unique<Button>("I", m_font);
  instToggle->setPosition(5.F, kTOP_BAR_HEIGHT + 130.F);
  instToggle->setSize(kTOGGLE_BUTTON_SIZE, kTOGGLE_BUTTON_SIZE);
  instToggle->setCallback([this]() {
    m_activeView = (m_activeView == ActiveView::INSTRUCTIONS)
                       ? ActiveView::NONE
                       : ActiveView::INSTRUCTIONS;
    m_needsUpdate = true;
  });
  m_buttons.push_back(std::move(instToggle));

  // Instruction Builder toggle
  auto builderToggle = std::make_unique<Button>("B", m_font);
  builderToggle->setPosition(5.F, kTOP_BAR_HEIGHT + 190.F);
  builderToggle->setSize(kTOGGLE_BUTTON_SIZE, kTOGGLE_BUTTON_SIZE);
  builderToggle->setCallback([this]() {
    m_activeView = (m_activeView == ActiveView::BUILDER) ? ActiveView::NONE
                                                         : ActiveView::BUILDER;
    m_needsUpdate = true;
  });
  m_buttons.push_back(std::move(builderToggle));

  // Instruction Queue toggle
  auto queueToggle = std::make_unique<Button>("Q", m_font);
  queueToggle->setPosition(5.F, kTOP_BAR_HEIGHT + 250.F);
  queueToggle->setSize(kTOGGLE_BUTTON_SIZE, kTOGGLE_BUTTON_SIZE);
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
  m_builderView->setPosition(kLEFT_SIDEBAR_WIDTH + 20.F,
                             kTOP_BAR_HEIGHT + 20.F);
  m_builderView->setSize(500.F, 240.F);
  m_queueView->setPosition(kLEFT_SIDEBAR_WIDTH + 540.F, kTOP_BAR_HEIGHT + 20.F);
  m_queueView->setSize(500.F, 300.F);

  // Wire callbacks
  m_builderView->setOnAdd([this](const std::string& line) {
    m_instructionQueue.push_back(line);
    InstructionCache::append(line);
    m_queueView->setItems(m_instructionQueue);
  });

  m_queueView->setOnDelete([this](size_t idx) {
    if (idx < m_instructionQueue.size()) {
      m_instructionQueue.erase(m_instructionQueue.begin() + idx);
      syncQueueToCache(m_instructionQueue);
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
      auto newIdx = static_cast<size_t>(static_cast<int>(idx) + delta);
      if (newIdx < m_instructionQueue.size()) {
        std::swap(m_instructionQueue[idx], m_instructionQueue[newIdx]);
        syncQueueToCache(m_instructionQueue);
        m_queueView->setItems(m_instructionQueue);
      }
    }
  });
  auto initialSize = m_window.getSize();
  handleResize(initialSize.x, initialSize.y);
}

bool CPUVisualizer::loadFont(sf::Font& font) {
  const std::array<std::string_view, 5> kFONT_PATHS = {
      "/usr/share/fonts/TTF/DejaVuSans.ttf",
      "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
      "/usr/share/fonts/liberation/LiberationSans-Regular.ttf",
      "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
      R"(C:\Windows\Fonts\arial.ttf)",
  };

  for (std::string_view path : kFONT_PATHS) {
    if (font.openFromFile(path)) {
      std::cout << "Loaded font from: " << path << "\n";
      if (!font.isSmooth()) { font.setSmooth(true); }
      return true;
    }
  }

  std::cerr << "ERROR: Could not load any font!\n";
  return false;
}

void CPUVisualizer::handleResize(unsigned width, unsigned height) {
  m_datapathView->setPosition(kLEFT_SIDEBAR_WIDTH, kTOP_BAR_HEIGHT);
  m_datapathView->setSize(width - kLEFT_SIDEBAR_WIDTH,
                          height - kTOP_BAR_HEIGHT);

  // TODO(evice): Resize other views

  m_needsUpdate = true;
  m_updateTopBar = true;
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

// TODO: Only draw when needed
void CPUVisualizer::draw() {
  // Draw layout sections
  if (m_updateTopBar) { drawTopBar(); }
  if (m_runningInstructions) { drawTopBarText(); }
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
  ExecutionStage stage = m_cpu.getCurrentStage();
  std::string stageText = std::string(stageToString(stage));

  sf::Text stageDisplay(m_font);
  stageDisplay.setString(stageText);
  stageDisplay.setCharacterSize(16);
  stageDisplay.setFillColor(kTITLE_TEXT_COLOR);
  stageDisplay.setPosition({20.F, 20.F});
  m_window.draw(stageDisplay);
}

void CPUVisualizer::handleMouseMove(float x, float y) {
  for (auto& btn : m_buttons) {
    btn->handleMouseMove(x, y);
  }
  if (m_activeView == ActiveView::BUILDER && m_builderView) {
    m_builderView->handleMouseMove(x, y);
  }
  if (m_activeView == ActiveView::QUEUE && m_queueView) {
    m_queueView->handleMouseMove(x, y);
  }
}

void CPUVisualizer::handleMousePress(float x, float y) {
  for (auto& btn : m_buttons) {
    btn->handleMousePress(x, y);
  }
  if (m_activeView == ActiveView::BUILDER && m_builderView) {
    m_builderView->handleMousePress(x, y);
  }
  if (m_activeView == ActiveView::QUEUE && m_queueView) {
    m_queueView->handleMousePress(x, y);
  }
}

void CPUVisualizer::handleMouseRelease(float x, float y) {
  for (auto& btn : m_buttons) {
    btn->handleMouseRelease(x, y);
  }
  if (m_activeView == ActiveView::BUILDER && m_builderView) {
    m_builderView->handleMouseRelease(x, y);
  }
  if (m_activeView == ActiveView::QUEUE && m_queueView) {
    m_queueView->handleMouseRelease(x, y);
  }
}

void CPUVisualizer::handleMouseWheel(float x, float y, float delta) {
  if (m_activeView == ActiveView::BUILDER && m_builderView) {
    m_builderView->handleMouseWheel(x, y, delta);
  }
}

void CPUVisualizer::drawTopBarText() {
  drawPipelineStage();

  sf::Text pcText(m_font);
  std::ostringstream pcStream;
  pcStream << "PC: 0x" << std::hex << std::setw(8) << std::setfill('0')
           << m_cpu.getRegisters().getPc();
  pcText.setString(pcStream.str());
  pcText.setCharacterSize(16);
  pcText.setFillColor(kTITLE_TEXT_COLOR);
  pcText.setPosition({300.F, 20.F});
  m_window.draw(pcText);

  word_t pc = m_cpu.getRegisters().getPc();
  word_t instr = m_cpu.getMemory().readWord(pc);
  Decoder::InstructionDetails decoded = Decoder::getDetails(instr);

  std::string instruction = decoded.mnemonic;

  for (std::string& field : decoded.fields) {
    instruction += field;
  }

  sf::Text instrText(m_font, "Instruction: " + instruction, 16);
  instrText.setFillColor(kTITLE_TEXT_COLOR);
  instrText.setPosition({500.F, 20.F});
  m_window.draw(instrText);
}

void CPUVisualizer::drawTopBar() {
  auto windowSize = m_window.getSize();

  sf::RectangleShape topBar(
      {static_cast<float>(windowSize.x), kTOP_BAR_HEIGHT});
  topBar.setPosition({0, 0});
  topBar.setFillColor(kVIEW_BOX_BACKGROUND_COLOR);
  topBar.setOutlineColor(kVIEW_BOX_OUTLINE_COLOR);
  topBar.setOutlineThickness(2.F);
  m_window.draw(topBar);

  drawTopBarText();
}

void CPUVisualizer::drawLeftSidebar() {
  auto windowSize = m_window.getSize();

  // Draw sidebar background
  sf::RectangleShape sidebar(
      {kLEFT_SIDEBAR_WIDTH,
       static_cast<float>(windowSize.y - kTOP_BAR_HEIGHT)});
  sidebar.setPosition({0, kTOP_BAR_HEIGHT});
  sidebar.setFillColor(kVIEW_BOX_BACKGROUND_COLOR);
  m_window.draw(sidebar);
}

void CPUVisualizer::drawMainArea() {
  auto windowSize = m_window.getSize();

  // Draw main area background
  sf::RectangleShape mainArea(
      {static_cast<float>(windowSize.x - kLEFT_SIDEBAR_WIDTH),
       static_cast<float>(windowSize.y - kTOP_BAR_HEIGHT)});
  mainArea.setPosition({kLEFT_SIDEBAR_WIDTH, kTOP_BAR_HEIGHT});
  mainArea.setFillColor(kDATA_PATH_BACKGROUND_COLOR);
  m_window.draw(mainArea);

  // Draw the datapath diagram
  m_datapathView->draw(m_window);
  // m_datapathView->drawScaffolding(m_window, mainArea.getSize());
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
    if (m_builderView) { m_builderView->draw(m_window); }
    if (m_queueView) { m_queueView->draw(m_window); }
    break;
  case ActiveView::QUEUE:
    if (m_queueView) { m_queueView->draw(m_window); }
    break;
  case ActiveView::NONE:
    // Nothing to draw
    break;
  }
}

void CPUVisualizer::handleTextEntered(uint32_t codepoint) {
  bool builderActive =
      (m_activeView == ActiveView::BUILDER && m_builderView != nullptr);
  if (builderActive) { m_builderView->handleTextEntered(codepoint); }

  // Global hotkeys via text input (only for printable characters)
  char ch = static_cast<char>(codepoint);
  if (std::isprint(static_cast<unsigned char>(ch)) != 0) {
    char c = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));

    // If builder is active and not typing and no dropdown open, allow
    // builder-specific hotkeys
    if (builderActive && !m_builderView->textActive()
        && !m_builderView->anyDropdownOpen()) {
      if (c == 'o') {
        m_builderView->openOp();
        return;
      }
      if (c == '1') {
        m_builderView->openRs();
        return;
      }
      if (c == '2') {
        m_builderView->openRt();
        return;
      }
      if (c == '3') {
        m_builderView->openRd();
        return;
      }
    }

    // View toggles (work regardless of active view unless typing text in
    // builder)
    if (!builderActive
        || (!m_builderView->textActive()
            && !m_builderView->anyDropdownOpen())) {
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

void CPUVisualizer::syncQueueToCache(
    std::vector<std::string>& instructionQueue) {
  InstructionCache::save(instructionQueue);
}

} // namespace ez_arch
