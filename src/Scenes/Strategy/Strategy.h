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

      // Track the currently viewed state
      unsigned int currentState_;

      // Store all states for the game
      std::vector<GameState> states_;

      // Current map to start with
      Map currentMap_;

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

      // Translate coords into map index
      static unsigned int coordToIndex(const Map& m, const Coord& coord);

      // Translate map index into a coord
      static Coord indexToCoord(const Map& m, unsigned int index);

      // Check if coordinates are valid
      static bool validateCoords(
          const Map& map, 
          const Coord& coords);

      // Collect the participating teams
      static std::map<Team, unsigned int> countTeams(const Map& map);

      // Get an object on the play field
      static std::pair<Team, Object> readMap(
          const Map& m, 
          const Coord& pos);

      // Update the map with new information
      static std::pair<bool, Map> updateMap(
          const Map& m, 
          const Coord& pos, 
          const Object& obj, 
          const Team& team);

      ///////////////////////////////////////////
      // IMPURE FUNCTIONS:
      // - Mutate the state of the scene
      ///////////////////////////////////////////
    
      // Resets the state of tic-tac-toe back to the beginning
      void resetGame();

      // Get a gamestate safely
      std::pair<bool, const GameState> getState(unsigned int n) const;

      ///////////////////////////////////////////
      // GRAPHICAL / LOGGING:
      // - Non-logic pure or impure functions
      ///////////////////////////////////////////

      // Adjust graphics for current game size
      void resizeGame();

      // Render an object in on the field
      void renderObject(
          sf::RenderWindow& window, 
          const Strategy::Object& object,
          const sf::Vector2u& coords);
  };
}

#endif
