#pragma once

#include <SFML/Graphics.hpp>
#include "core/cpu.hpp"
#include "gui/register_view.hpp"
#include <memory>

namespace ez_arch {

  class CPUVisualizer {
  public:
    CPUVisualizer(CPU& cpu, sf::RenderWindow& window);

    void update();
    void draw();

  private:
    CPU& cpu_;
    sf::RenderWindow& window_;
    sf::Font font_;
    
    // View components
    std::unique_ptr<RegisterView> registerView_;
  
    void drawPipelineStage();
    bool loadFont();
  };

} // namespace ez_arch
