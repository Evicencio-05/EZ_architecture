#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

#include "core/cpu.hpp"
#include "gui/button.hpp"
#include "gui/datapath_view.hpp"
#include "gui/instruction_builder_view.hpp"
#include "gui/instruction_cache.hpp"
#include "gui/instruction_queue_view.hpp"
#include "gui/instruction_view.hpp"
#include "gui/memory_view.hpp"
#include "gui/register_view.hpp"

namespace ez_arch {

class CPUVisualizer {
 public:
  CPUVisualizer(CPU& cpu, sf::RenderWindow& window);

  void update();
  void draw();

  // Event handling
  void handle_mouse_move(float x, float y);
  void handle_mouse_press(float x, float y);
  void handle_mouse_release(float x, float y);
  void handle_mouse_wheel(float x, float y, float delta);
  void handle_resize(unsigned width, unsigned height);
  // Keyboard/text input routed from main
  void handle_text_entered(uint32_t codepoint);
  void handle_key_pressed(int key_code);

 private:
  CPU& m_cpu;
  sf::RenderWindow& m_window;
  sf::Font m_font;
  bool m_needsUpdate;

  // Layout helpers
  void draw_top_bar();
  void draw_left_sidebar();
  void draw_main_area();
  void draw_active_view();

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
  void sync_queue_to_cache();

  // View state
  enum class ActiveView {
    NONE,
    REGISTERS,
    MEMORY,
    INSTRUCTIONS,
    BUILDER,
    QUEUE
  };
  ActiveView m_activeView;

  void draw_pipeline_stage();
  bool load_font();
};

}  // namespace ez_arch
