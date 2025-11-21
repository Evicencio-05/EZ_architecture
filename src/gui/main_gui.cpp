#include <SFML/Graphics.hpp>
#include <optional>

#include "core/cpu.hpp"
#include "gui/cpu_visualizer.hpp"

int main() {
  sf::ContextSettings settings;
  settings.antiAliasingLevel = 8;
  sf::RenderWindow window(sf::VideoMode({1200, 800}),
                          "EZ Architecture - MIPS Visualizer", sf::State::Windowed, settings);
  window.setFramerateLimit(15);

  // Create the CPU instance
  ez_arch::CPU cpu;

  // Start with empty program; use the Builder/Queue to construct instructions
  // at runtime.

  // Create the visualizer that will display the CPU state
  ez_arch::CPUVisualizer visualizer(cpu, window);

  while (window.isOpen()) {
    while (const std::optional kEVENT = window.pollEvent()) {
      if (kEVENT->is<sf::Event::Closed>()) { window.close();
}

      if (kEVENT->is<sf::Event::Resized>()) {
        auto size = kEVENT->getIf<sf::Event::Resized>()->size;
        window.setView(sf::View(sf::FloatRect({0, 0}, sf::Vector2f(size))));
        visualizer.handleResize(size.x, size.y);
      }

      // Handle mouse events
      if (kEVENT->is<sf::Event::MouseMoved>()) {
        auto pos = kEVENT->getIf<sf::Event::MouseMoved>()->position;
        visualizer.handleMouseMove(pos.x, pos.y);
      }

      if (kEVENT->is<sf::Event::MouseButtonPressed>()) {
        auto pos = kEVENT->getIf<sf::Event::MouseButtonPressed>()->position;
        visualizer.handleMousePress(pos.x, pos.y);
      }

      if (kEVENT->is<sf::Event::MouseButtonReleased>()) {
        auto pos = kEVENT->getIf<sf::Event::MouseButtonReleased>()->position;
        visualizer.handleMouseRelease(pos.x, pos.y);
      }
      if (kEVENT->is<sf::Event::MouseWheelScrolled>()) {
        const auto *ev = kEVENT->getIf<sf::Event::MouseWheelScrolled>();
        visualizer.handleMouseWheel(ev->position.x, ev->position.y, ev->delta);
      }

      // Keyboard/text events (for builder input)
      if (kEVENT->is<sf::Event::TextEntered>()) {
        auto unicode = kEVENT->getIf<sf::Event::TextEntered>()->unicode;
        visualizer.handleTextEntered(unicode);
      }
      if (kEVENT->is<sf::Event::KeyPressed>()) {
        auto code =
            static_cast<int>(kEVENT->getIf<sf::Event::KeyPressed>()->code);
        visualizer.handleKeyPressed(code);
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
