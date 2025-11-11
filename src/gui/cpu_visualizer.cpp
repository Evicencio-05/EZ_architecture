#include "gui/cpu_visualizer.hpp"
#include "gui/style.hpp"
#include "core/decoder.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <iostream>

namespace ez_arch {

  CPUVisualizer::CPUVisualizer(CPU& cpu, sf::RenderWindow& window) :
    m_cpu(cpu), m_window(window) {

      if (!loadFont()) {
        std::cerr << "Warning: Could not load font. Text will not display.\n";
      }
      
      // Initialize view state
      m_activeView = ActiveView::NONE;
      
      // Create the register view
      m_registerView = std::make_unique<RegisterView>(m_cpu.get_registers(), m_font);
      m_registerView->setPosition(LEFT_SIDEBAR_WIDTH + 20.f, TOP_BAR_HEIGHT + 20.f);
      
      // Create the memory view
      m_memoryView = std::make_unique<MemoryView>(m_cpu.get_memory(), m_cpu.get_registers(), m_font);
      m_memoryView->setPosition(LEFT_SIDEBAR_WIDTH + 20.f, TOP_BAR_HEIGHT + 20.f);
      m_memoryView->setDisplayRange(0, 16);
      
      // Create the instruction view
      m_instructionView = std::make_unique<InstructionView>(m_cpu.get_memory(), m_cpu.get_registers(), m_font);
      m_instructionView->setPosition(LEFT_SIDEBAR_WIDTH + 20.f, TOP_BAR_HEIGHT + 20.f);
      m_instructionView->setDisplayRange(0, 16);
      
      // Create the datapath view for main area
      m_datapathView = std::make_unique<DatapathView>(m_cpu, m_font);

      // Create control buttons - positioned in top bar
      auto stepStageBtn = std::make_unique<Button>("Stage", m_font);
      stepStageBtn->setPosition(710.f, 15.f);
      stepStageBtn->setCallback([this]() { m_cpu.step_stage(); });
      m_buttons.push_back(std::move(stepStageBtn));
      
      auto stepBtn = std::make_unique<Button>("Step", m_font);
      stepBtn->setPosition(800.f, 15.f);
      stepBtn->setCallback([this]() { m_cpu.step(); });
      m_buttons.push_back(std::move(stepBtn));
      
      auto runBtn = std::make_unique<Button>("Run", m_font);
      runBtn->setPosition(890.f, 15.f);
      runBtn->setCallback([this]() { m_cpu.run(); });
      m_buttons.push_back(std::move(runBtn));
      
      auto resetBtn = std::make_unique<Button>("Reset", m_font);
      resetBtn->setPosition(980.f, 15.f);
      resetBtn->setCallback([this]() { m_cpu.reset(); });
      m_buttons.push_back(std::move(resetBtn));
      
      // Create toggle buttons - positioned in left sidebar
      auto regToggle = std::make_unique<Button>("R", m_font);
      regToggle->setPosition(5.f, TOP_BAR_HEIGHT + 10.f);
      regToggle->setSize(TOGGLE_BUTTON_SIZE, TOGGLE_BUTTON_SIZE);
      regToggle->setCallback([this]() { 
        m_activeView = (m_activeView == ActiveView::REGISTERS) ? ActiveView::NONE : ActiveView::REGISTERS; 
      });
      m_buttons.push_back(std::move(regToggle));
      
      auto memToggle = std::make_unique<Button>("M", m_font);
      memToggle->setPosition(5.f, TOP_BAR_HEIGHT + 70.f);
      memToggle->setSize(TOGGLE_BUTTON_SIZE, TOGGLE_BUTTON_SIZE);
      memToggle->setCallback([this]() { 
        m_activeView = (m_activeView == ActiveView::MEMORY) ? ActiveView::NONE : ActiveView::MEMORY; 
      });
      m_buttons.push_back(std::move(memToggle));
      
      auto instToggle = std::make_unique<Button>("I", m_font);
      instToggle->setPosition(5.f, TOP_BAR_HEIGHT + 130.f);
      instToggle->setSize(TOGGLE_BUTTON_SIZE, TOGGLE_BUTTON_SIZE);
      instToggle->setCallback([this]() { 
        m_activeView = (m_activeView == ActiveView::INSTRUCTIONS) ? ActiveView::NONE : ActiveView::INSTRUCTIONS; 
      });
      m_buttons.push_back(std::move(instToggle));
  }

  bool CPUVisualizer::loadFont() {

    const char* fontPaths[] = {
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/home/evice/.local/share/fonts/omarchy.ttf"
    };

    for (const char* path : fontPaths) {
      if (m_font.openFromFile(path)) {
        std::cout << "Loaded font from: " << path << "\n";
        return true;
      }
    }

    std::cerr << "ERROR: Could not load any font!\n";
    return false;
  }

  void CPUVisualizer::update() {
    // Update all view components
    m_registerView->update();
    m_memoryView->update();
    m_instructionView->update();
    m_datapathView->update();
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
  }
  
  void CPUVisualizer::handleMousePress(float x, float y) {
    for (auto& btn : m_buttons) {
      btn->handleMousePress(x, y);
    }
  }
  
  void CPUVisualizer::handleMouseRelease(float x, float y) {
    for (auto& btn : m_buttons) {
      btn->handleMouseRelease(x, y);
    }
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
    pcStream << "PC: 0x" << std::hex << std::setw(8) << std::setfill('0') << m_cpu.get_registers().get_pc();
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
    sf::RectangleShape sidebar({
      LEFT_SIDEBAR_WIDTH, 
      static_cast<float>(windowSize.y - TOP_BAR_HEIGHT)
    });
    sidebar.setPosition({0, TOP_BAR_HEIGHT});
    sidebar.setFillColor(VIEW_BOX_BACKGROUND_COLOR);
    m_window.draw(sidebar);
  }
  
  void CPUVisualizer::drawMainArea() {
    auto windowSize = m_window.getSize();
    
    // Draw main area background
    sf::RectangleShape mainArea({
      static_cast<float>(windowSize.x - LEFT_SIDEBAR_WIDTH),
      static_cast<float>(windowSize.y - TOP_BAR_HEIGHT)
    });
    mainArea.setPosition({LEFT_SIDEBAR_WIDTH, TOP_BAR_HEIGHT});
    mainArea.setFillColor(DATA_PATH_BACKGROUND_COLOR);
    m_window.draw(mainArea);
    
    // Draw the datapath diagram
    m_datapathView->setPosition(LEFT_SIDEBAR_WIDTH, TOP_BAR_HEIGHT);
    m_datapathView->setSize(
      windowSize.x - LEFT_SIDEBAR_WIDTH,
      windowSize.y - TOP_BAR_HEIGHT
    );
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
      case ActiveView::NONE:
        // Nothing to draw
        break;
    }
  }

} // namespace ez_arch
