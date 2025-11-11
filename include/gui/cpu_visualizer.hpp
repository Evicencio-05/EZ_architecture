#pragma once

#include <SFML/Graphics.hpp>
#include "core/cpu.hpp"
#include "gui/register_view.hpp"
#include "gui/memory_view.hpp"
#include "gui/instruction_view.hpp"
#include "gui/button.hpp"
#include <memory>
#include <vector>

namespace ez_arch {

  class CPUVisualizer {
  public:
    CPUVisualizer(CPU& cpu, sf::RenderWindow& window);

    void update();
    void draw();
    
    // Event handling
    void handleMouseMove(float x, float y);
    void handleMousePress(float x, float y);
    void handleMouseRelease(float x, float y);

  private:
    CPU& cpu_;
    sf::RenderWindow& window_;
    sf::Font font_;
    
    // Layout helpers
    void drawTopBar();
    void drawLeftSidebar();
    void drawMainArea();
    void drawActiveView();
    
    // View components
    std::unique_ptr<RegisterView> registerView_;
    std::unique_ptr<MemoryView> memoryView_;
    std::unique_ptr<InstructionView> instructionView_;
    
    // Control buttons
    std::vector<std::unique_ptr<Button>> buttons_;
    
    // View state
    enum class ActiveView { NONE, REGISTERS, MEMORY, INSTRUCTIONS };
    ActiveView activeView_;
  
    void drawPipelineStage();
    bool loadFont();
  };

} // namespace ez_arch
