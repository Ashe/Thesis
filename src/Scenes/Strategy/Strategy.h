// Strategy.h
// A strategy game for testing AI

#ifndef STRATEGY_H
#define STRATEGY_H

#include <cmath>

#include "../../Scene.h"
#include "../../Controller/Common.h"
#include "../../Resources.h"

#include "Common.h"
#include "GameState.h"
#include "Map.h"
#include "Action.h"

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

      // Currently hovered tile by mouse
      Coord hoveredTile_;

      // Controllers used to play the game
      std::map<Team, Controller::Type> controllers_;
      const Controller::Type defaultController_ = Controller::Type::Human;

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

      // Whether to show or hide the editor
      bool enableEditor_ = false;

      ///////////////////////////////////////////
      // PURE FUNCTIONS:
      // - Functions without side effects
      // - Used to transform or read game states
      ///////////////////////////////////////////

      // Attempt to take action on a gamestate
      static std::pair<bool, GameState> takeAction(
          const GameState& state,
          const Action& action);

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

      // Get the controller for a team
      Controller::Type getController(const Team& team) const;

      // Get the reference to the controller for a team
      Controller::Type& getControllerRef(const Team& team);

      ///////////////////////////////////////////
      // GRAPHICAL / LOGGING:
      // - Non-logic pure or impure functions
      ///////////////////////////////////////////

      // Adjust graphics for current game size
      void resizeGame();

      // Render an object in on the field
      void renderObject(
          sf::RenderWindow& window, 
          const Team& team,
          const Object& object,
          const Coord& coords,
          const RenderStyle& style);

      // Get the colour associated with a team
      static sf::Color getTeamColour(const GameState& state, const Team& team);
  };
}

#endif
