#pragma once

#include "core/cpu.hpp"
#include "gui/button.hpp"
#include "gui/datapath_view.hpp"
#include "gui/instruction_builder_view.hpp"
#include "gui/instruction_queue_view.hpp"
#include "gui/instruction_view.hpp"
#include "gui/memory_view.hpp"
#include "gui/register_view.hpp"

#include <SFML/Graphics.hpp>
#include <cstdint>
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
  void handleMouseWheel(float x, float y, float delta);
  void handleResize(unsigned width, unsigned height);
  // Keyboard/text input routed from main
  void handleTextEntered(uint32_t codepoint);
  void handleKeyPressed(int keyCode);

private:
  CPU& m_cpu;
  sf::RenderWindow& m_window;
  sf::Font m_font;
  bool m_needsUpdate;

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
  std::unique_ptr<InstructionBuilderView> m_builderView;
  std::unique_ptr<InstructionQueueView> m_queueView;

  // Control buttons
  std::vector<std::unique_ptr<Button>> m_buttons;

  // Instruction queue (persistent)
  std::vector<std::string> m_instructionQueue;
  static void syncQueueToCache();

  // View state
  enum class ActiveView : uint8_t {
    NONE,
    REGISTERS,
    MEMORY,
    INSTRUCTIONS,
    BUILDER,
    QUEUE
  };
  ActiveView m_activeView;

  void drawPipelineStage();
  bool loadFont();
};

} // namespace ez_arch
