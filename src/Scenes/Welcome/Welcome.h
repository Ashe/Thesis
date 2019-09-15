// Welcome.h
// A welcome scene for the application

#ifndef WELCOME_H
#define WELCOME_H

#include "../../Scene.h"

// Simply direct the user to the scene switcher
class WelcomeScene: public Scene {
  public:

    // When scene starts
    void onBegin() override {
      auto size = circle_.getLocalBounds();
      circle_.setOrigin(size.width * 0.5f, size.height * 0.5f);
    }

    // Perform logic every frame
    void onUpdate(const sf::Time& dt) override {

      // Move circle towards mouse
      const auto circlePos = circle_.getPosition();
      const auto mousePos = App::getMousePosition();
      const auto speed = applySpeedMultiplier? 1.5f : 0.3f;
      const auto velocity = (mousePos - circlePos) * speed;
      circle_.setPosition(circlePos + (velocity * dt.asSeconds()));
    }

    // Basic debug rendering
    void onRender(sf::RenderWindow& window) override {
      window.draw(circle_);
    }

    // Handle SFML events
    void onEvent(const sf::Event& event) override {

      // Check for mouse events
      if (event.type == sf::Event::MouseButtonPressed) {

        // If it was left mouse
        if (event.mouseButton.button == sf::Mouse::Left) {
          applySpeedMultiplier = true;
        }
      }

      // Mouse release
      else if (event.type == sf::Event::MouseButtonReleased) {

        // If it was left mouse
        if (event.mouseButton.button == sf::Mouse::Left) {
          applySpeedMultiplier = false;
        }
      }

      // Check for key events
      else if (event.type == sf::Event::KeyPressed) {
        Console::log("Key press detected: %d", event.key.code);
      }
    }

    // Show location of circle on details
    void addDebugDetails() override {
      ImGui::Begin("Welcome!");
      ImGui::Text("Use the scene switcher in the Debug Menu to navigate.");
      ImGui::Spacing();
      auto pos = circle_.getPosition();
      ImGui::Text("Circle location: (%d, %d)", (int)pos.x, (int)pos.y);
      ImGui::Text("Boosting?: %s", 
          applySpeedMultiplier ? "true" : "false");
      ImGui::End();
    }

  private:

    // Circle to render
    sf::CircleShape circle_ = sf::CircleShape(50);

    // Speed multiplier
    bool applySpeedMultiplier = false;
};

#endif
