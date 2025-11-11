#pragma once

#include <SFML/Graphics.hpp>
#include "core/cpu.hpp"
#include "gui/register_view.hpp"
#include "gui/memory_view.hpp"
#include "gui/instruction_view.hpp"
#include "gui/datapath_view.hpp"
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
    CPU& m_cpu;
    sf::RenderWindow& m_window;
    sf::Font m_font;
    
    // Layout helpers
    void drawTopBar();
    void drawLeftSidebar();
    void drawMainArea();
    void drawActiveView();
    
    // View components
    std::unique_ptr<RegisterView> m_registerView;
    std::unique_ptr<MemoryView> m_memoryView;
    std::unique_ptr<InstructionView> m_instructionView;
    std::unique_ptr<DatapathView> m_datapathView;
    
    // Control buttons
    std::vector<std::unique_ptr<Button>> m_buttons;
    
    // View state
    enum class ActiveView { NONE, REGISTERS, MEMORY, INSTRUCTIONS };
    ActiveView m_activeView;
  
    void drawPipelineStage();
    bool loadFont();
  };

} // namespace ez_arch
