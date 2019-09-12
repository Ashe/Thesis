// Test.h
// A basic scene for testing the app engine

#ifndef TEST_H
#define TEST_H

#include "../../Scene.h"

// Child of Scene class
class TestScene: public Scene {
  public:

    // When scene starts
    void begin() override {
      Scene::begin();
      Console::log("Beginning testing..");
    }

    // Perform logic every frame
    void update(const sf::Time& dt) override {
      Scene::update(dt);

      // Move circle towards mouse
      const auto circlePos = circle_.getPosition();
      const auto mousePos = App::getMousePosition();
      const auto speed = applySpeedMultiplier? 1.5f : 0.3f;
      const auto velocity = (mousePos - circlePos) * speed;
      circle_.setPosition(circlePos + (velocity * dt.asSeconds()));
    }

    // Basic debug rendering
    void render(sf::RenderWindow& window) override {
      Scene::render(window);
      window.draw(circle_);
    }

    // Handle SFML events
    void handleEvent(const sf::Event& event) override {
      Scene::handleEvent(event);

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
      ImGui::Begin("Debug");
      auto pos = circle_.getPosition();
      ImGui::Text("Circle location: (%f, %f)", pos.x, pos.y);
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
