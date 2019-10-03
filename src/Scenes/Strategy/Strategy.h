// Strategy.h
// A strategy game for testing AI

#ifndef STRATEGY_H
#define STRATEGY_H

#include "../../Scene.h"
#include "../../Resources.h"

#include "GameState.h"

// Encapsulate Strategy related classes
namespace Strategy {

  // Game scene for strategy game
  class Game : public Scene {
    public:

      ///////////////////////////////////////////
      // SCENE FUNCTIONS:
      // - Mandatory functions for the scene
      ///////////////////////////////////////////

      // When the scene starts set up a game
      void onBegin() override;

      // Update the currently hovered tile
      void onUpdate(const sf::Time& dt) override;

      // Handle input and game size changes
      void onEvent(const sf::Event& event) override;

      // Render the render the game board and state
      void onRender(sf::RenderWindow& window) override;

      // Whenever the scene is re-shown, ensure graphics are correct
      void onShow() override;

      // Add details to debug windows
      void addDebugDetails() override;

    private:

      // @TODO: Update this
      // Temporary state for drawing
      GameState currentState_;

      // The grid of the playing field to draw
      sf::VertexArray grid_;

      // Sizes and positions of game graphics
      float maxGameLength_;
      float tileLength_;
      sf::Vector2f center_;
      float left_;
      float top_;
      float right_;
      float bottom_;

      ///////////////////////////////////////////
      // PURE FUNCTIONS:
      // - Functions without side effects
      // - Used to transform or read game states
      ///////////////////////////////////////////
      

      ///////////////////////////////////////////
      // IMPURE FUNCTIONS:
      // - Mutate the state of the scene
      ///////////////////////////////////////////
      

      ///////////////////////////////////////////
      // GRAPHICAL / LOGGING:
      // - Non-logic pure or impure functions
      ///////////////////////////////////////////

      // Adjust graphics for current game size
      void resizeGame();

      // Render an object in on the field
      void renderObject(const sf::Vector2u& coords);
  };
}

#endif
