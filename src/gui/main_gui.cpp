#include <SFML/Graphics.hpp>
#include <optional>
#include "core/cpu.hpp"
#include "gui/cpu_visualizer.hpp"

int main() {
 
  sf::RenderWindow window(sf::VideoMode({1200, 800}), "EZ Architecture - MIPS Visualizer");
  window.setFramerateLimit(20);
  
  // Create the CPU instance
  ez_arch::CPU cpu;
  
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
    }

    window.clear(sf::Color(255, 255, 255));
    
    // Update visualizer state from CPU
    visualizer.update();
    
    // Draw all GUI components
    visualizer.draw();

    window.display();
  }

} 
