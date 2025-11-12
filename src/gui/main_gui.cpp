#include <SFML/Graphics.hpp>
#include <optional>
#include "core/cpu.hpp"
#include "gui/cpu_visualizer.hpp"

int main() {
 
  sf::RenderWindow window(sf::VideoMode({1200, 800}), "EZ Architecture - MIPS Visualizer");
  window.setFramerateLimit(2);
  
  // Create the CPU instance
  ez_arch::CPU cpu;
  
  // Load a simple test program
  // add $t0, $t1, $t2  (0x012A4020)
  // lw $t3, 0($t0)     (0x8D0B0000)
  // sw $t3, 4($t0)     (0xAD0B0004)
  std::vector<ez_arch::word_t> testProgram = {
    0x012A4020,  // add $t0, $t1, $t2
    0x8D0B0000,  // lw $t3, 0($t0)
    0xAD0B0004,  // sw $t3, 4($t0)
    0x00000000   // nop (halt)
  };
  cpu.load_program(testProgram);
  
  // Create the visualizer that will display the CPU state
  ez_arch::CPUVisualizer visualizer(cpu, window);

  while (window.isOpen()) {
    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>())
        window.close();

      if (event->is<sf::Event::Resized>()) {
        auto size = event->getIf<sf::Event::Resized>()->size;
        window.setView(sf::View(sf::FloatRect({0,0}, sf::Vector2f(size))));
      }
      
      // Handle mouse events
      if (event->is<sf::Event::MouseMoved>()) {
        auto pos = event->getIf<sf::Event::MouseMoved>()->position;
        visualizer.handleMouseMove(pos.x, pos.y);
      }
      
      if (event->is<sf::Event::MouseButtonPressed>()) {
        auto pos = event->getIf<sf::Event::MouseButtonPressed>()->position;
        visualizer.handleMousePress(pos.x, pos.y);
      }
      
      if (event->is<sf::Event::MouseButtonReleased>()) {
        auto pos = event->getIf<sf::Event::MouseButtonReleased>()->position;
        visualizer.handleMouseRelease(pos.x, pos.y);
      }
    }

    window.clear(sf::Color(255, 255, 255));
    
    // Update visualizer state from CPU
    visualizer.update();
    
    // Draw all GUI components
    visualizer.draw();

    window.display();
  }

} 
