#include "gui/cpu_visualizer.hpp"
#include "gui/style.hpp"
#include "core/decoder.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <iostream>

namespace ez_arch {

  CPUVisualizer::CPUVisualizer(CPU& cpu, sf::RenderWindow& window) :
    cpu_(cpu), window_(window) {

      if (!loadFont()) {
        std::cerr << "Warning: Could not load font. Text will not display.\n";
      }
      
      // Initialize view state
      activeView_ = ActiveView::NONE;
      
      // Create the register view
      registerView_ = std::make_unique<RegisterView>(cpu_.get_registers(), font_);
      registerView_->setPosition(LEFT_SIDEBAR_WIDTH + 20.f, TOP_BAR_HEIGHT + 20.f);
      
      // Create the memory view
      memoryView_ = std::make_unique<MemoryView>(cpu_.get_memory(), cpu_.get_registers(), font_);
      memoryView_->setPosition(LEFT_SIDEBAR_WIDTH + 20.f, TOP_BAR_HEIGHT + 20.f);
      memoryView_->setDisplayRange(0, 16);
      
      // Create the instruction view
      instructionView_ = std::make_unique<InstructionView>(cpu_.get_memory(), cpu_.get_registers(), font_);
      instructionView_->setPosition(LEFT_SIDEBAR_WIDTH + 20.f, TOP_BAR_HEIGHT + 20.f);
      instructionView_->setDisplayRange(0, 16);
      
      // Create control buttons - positioned in top bar
      auto stepBtn = std::make_unique<Button>("Step", font_);
      stepBtn->setPosition(800.f, 15.f);
      stepBtn->setCallback([this]() { cpu_.step(); });
      buttons_.push_back(std::move(stepBtn));
      
      auto runBtn = std::make_unique<Button>("Run", font_);
      runBtn->setPosition(890.f, 15.f);
      runBtn->setCallback([this]() { cpu_.run(); });
      buttons_.push_back(std::move(runBtn));
      
      auto resetBtn = std::make_unique<Button>("Reset", font_);
      resetBtn->setPosition(980.f, 15.f);
      resetBtn->setCallback([this]() { cpu_.reset(); });
      buttons_.push_back(std::move(resetBtn));
      
      // Create toggle buttons - positioned in left sidebar
      auto regToggle = std::make_unique<Button>("R", font_);
      regToggle->setPosition(5.f, TOP_BAR_HEIGHT + 10.f);
      regToggle->setSize(TOGGLE_BUTTON_SIZE, TOGGLE_BUTTON_SIZE);
      regToggle->setCallback([this]() { 
        activeView_ = (activeView_ == ActiveView::REGISTERS) ? ActiveView::NONE : ActiveView::REGISTERS; 
      });
      buttons_.push_back(std::move(regToggle));
      
      auto memToggle = std::make_unique<Button>("M", font_);
      memToggle->setPosition(5.f, TOP_BAR_HEIGHT + 70.f);
      memToggle->setSize(TOGGLE_BUTTON_SIZE, TOGGLE_BUTTON_SIZE);
      memToggle->setCallback([this]() { 
        activeView_ = (activeView_ == ActiveView::MEMORY) ? ActiveView::NONE : ActiveView::MEMORY; 
      });
      buttons_.push_back(std::move(memToggle));
      
      auto instToggle = std::make_unique<Button>("I", font_);
      instToggle->setPosition(5.f, TOP_BAR_HEIGHT + 130.f);
      instToggle->setSize(TOGGLE_BUTTON_SIZE, TOGGLE_BUTTON_SIZE);
      instToggle->setCallback([this]() { 
        activeView_ = (activeView_ == ActiveView::INSTRUCTIONS) ? ActiveView::NONE : ActiveView::INSTRUCTIONS; 
      });
      buttons_.push_back(std::move(instToggle));
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
      if (font_.openFromFile(path)) {
        std::cout << "Loaded font from: " << path << "\n";
        return true;
      }
    }

    std::cerr << "ERROR: Could not load any font!\n";
    return false;
  }

  void CPUVisualizer::update() {
    // Update all view components
    registerView_->update();
    memoryView_->update();
    instructionView_->update();
  }

  void CPUVisualizer::draw() {
    // Draw layout sections
    drawTopBar();
    drawLeftSidebar();
    drawMainArea();
    drawActiveView();
    
    // Draw buttons
    for (auto& btn : buttons_) {
      btn->draw(window_);
    }
  }

  void CPUVisualizer::drawPipelineStage() {
    // Compact version for top bar
    ExecutionStage stage = cpu_.get_current_stage();
    std::string stageText = std::string(stageToString(stage));
    
    sf::Text stageDisplay(font_);
    stageDisplay.setString(stageText);
    stageDisplay.setCharacterSize(16);
    stageDisplay.setFillColor(TITLE_TEXT_COLOR);
    stageDisplay.setPosition({20.f, 20.f});
    window_.draw(stageDisplay);
  }
  
  void CPUVisualizer::handleMouseMove(float x, float y) {
    for (auto& btn : buttons_) {
      btn->handleMouseMove(x, y);
    }
  }
  
  void CPUVisualizer::handleMousePress(float x, float y) {
    for (auto& btn : buttons_) {
      btn->handleMousePress(x, y);
    }
  }
  
  void CPUVisualizer::handleMouseRelease(float x, float y) {
    for (auto& btn : buttons_) {
      btn->handleMouseRelease(x, y);
    }
  }
  
  void CPUVisualizer::drawTopBar() {
    auto windowSize = window_.getSize();
    
    // Draw top bar background
    sf::RectangleShape topBar({static_cast<float>(windowSize.x), TOP_BAR_HEIGHT});
    topBar.setPosition({0, 0});
    topBar.setFillColor(VIEW_BOX_BACKGROUND_COLOR);
    topBar.setOutlineColor(VIEW_BOX_OUTLINE_COLOR);
    topBar.setOutlineThickness(2.f);
    window_.draw(topBar);
    
    // Draw pipeline stage (compact version)
    drawPipelineStage();
    
    // Draw PC info
    sf::Text pcText(font_);
    std::ostringstream pcStream;
    pcStream << "PC: 0x" << std::hex << std::setw(8) << std::setfill('0') << cpu_.get_registers().get_pc();
    pcText.setString(pcStream.str());
    pcText.setCharacterSize(16);
    pcText.setFillColor(TITLE_TEXT_COLOR);
    pcText.setPosition({300.f, 20.f});
    window_.draw(pcText);
    
    // Draw current instruction
    word_t pc = cpu_.get_registers().get_pc();
    word_t instr = cpu_.get_memory().read_word(pc);
    std::string decoded = Decoder::decode(instr);
    
    sf::Text instrText(font_);
    instrText.setString("Instruction: " + decoded);
    instrText.setCharacterSize(16);
    instrText.setFillColor(TITLE_TEXT_COLOR);
    instrText.setPosition({500.f, 20.f});
    window_.draw(instrText);
  }
  
  void CPUVisualizer::drawLeftSidebar() {
    auto windowSize = window_.getSize();
    
    // Draw sidebar background
    sf::RectangleShape sidebar({
      LEFT_SIDEBAR_WIDTH, 
      static_cast<float>(windowSize.y - TOP_BAR_HEIGHT)
    });
    sidebar.setPosition({0, TOP_BAR_HEIGHT});
    sidebar.setFillColor(VIEW_BOX_BACKGROUND_COLOR);
    window_.draw(sidebar);
  }
  
  void CPUVisualizer::drawMainArea() {
    auto windowSize = window_.getSize();
    
    // Draw main area background
    sf::RectangleShape mainArea({
      static_cast<float>(windowSize.x - LEFT_SIDEBAR_WIDTH),
      static_cast<float>(windowSize.y - TOP_BAR_HEIGHT)
    });
    mainArea.setPosition({LEFT_SIDEBAR_WIDTH, TOP_BAR_HEIGHT});
    mainArea.setFillColor(sf::Color(30, 30, 40));
    window_.draw(mainArea);
    
    // Draw placeholder text
    sf::Text placeholder(font_);
    placeholder.setString("Datapath Visualization Area\n(Future: Interactive CPU Diagram)");
    placeholder.setCharacterSize(24);
    placeholder.setFillColor(sf::Color(100, 100, 100));
    placeholder.setPosition({LEFT_SIDEBAR_WIDTH + 200.f, TOP_BAR_HEIGHT + 200.f});
    window_.draw(placeholder);
  }
  
  void CPUVisualizer::drawActiveView() {
    // Draw the selected view on top of the main area
    switch (activeView_) {
      case ActiveView::REGISTERS:
        registerView_->draw(window_);
        break;
      case ActiveView::MEMORY:
        memoryView_->draw(window_);
        break;
      case ActiveView::INSTRUCTIONS:
        instructionView_->draw(window_);
        break;
      case ActiveView::NONE:
        // Nothing to draw
        break;
    }
  }

} // namespace ez_arch
