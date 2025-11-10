#include "gui/cpu_visualizer.hpp"
#include "gui/style.hpp"
#include <iostream>

namespace ez_arch {

  CPUVisualizer::CPUVisualizer(CPU& cpu, sf::RenderWindow& window) :
    cpu_(cpu), window_(window) {

      if (!loadFont()) {
        std::cerr << "Warning: Could not load font. Text will not display.\n";
      }
      
      // Create the register view
      registerView_ = std::make_unique<RegisterView>(cpu_.get_registers(), font_);
      registerView_->setPosition(20.f, 140.f);  // Position it to the right of pipeline stage
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
  }

  void CPUVisualizer::draw() {
    // Draw all components
    drawPipelineStage();
    registerView_->draw(window_);
  }

  void CPUVisualizer::drawPipelineStage() {
    ExecutionStage stage = cpu_.get_current_stage();
    std::string stageText = std::string(stageToString(stage));


    float boxWidth = 250.f;
    float boxHeight = 80.f;
    float boxX = 20.f;
    float boxY = 20.f;

    sf::RectangleShape stageBox({boxWidth, boxHeight});
    stageBox.setPosition({boxX, boxY});
    stageBox.setFillColor(VIEW_BOX_BACKGROUND_COLOR);
    stageBox.setOutlineColor(VIEW_BOX_OUTLINE_COLOR);
    stageBox.setOutlineThickness(2.f);
    window_.draw(stageBox);


    sf::Text title(font_, "Current Pipeline Stage", 20);
    title.setFillColor(TITLE_TEXT_COLOR);


    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin({titleBounds.position.x + titleBounds.size.x / 2.0f,
                     titleBounds.position.y + titleBounds.size.y / 2.0f});
    title.setPosition({boxX + boxWidth / 2.0f, boxY + 25.f});
    window_.draw(title);


    sf::Text stageDisplay(font_, stageText, 24);
    stageDisplay.setFillColor(sf::Color(100, 120, 250));


    sf::FloatRect stageBounds = stageDisplay.getLocalBounds();
    stageDisplay.setOrigin({stageBounds.position.x + stageBounds.size.x / 2.0f,
                            stageBounds.position.y + stageBounds.size.y / 2.0f});
    stageDisplay.setPosition({boxX + boxWidth / 2.0f, boxY + 60.f});
    window_.draw(stageDisplay);
  }

} // namespace ez_arch
