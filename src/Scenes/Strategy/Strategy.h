// Strategy.h
// A strategy game for testing AI

#ifndef STRATEGY_H
#define STRATEGY_H

#include <future>
#include <thread>
#include <fstream>
#include <istream>
#include <tuple>
#include <algorithm>
#include <math.h>
#include <cmath>
#include <climits>

#include "../../Scene.h"
#include "../../Resources.h"
#include "../../Controller/Common.h"
#include "../../Controller/Random/Random.h"
#include "../../Controller/AStar/AStar.h"

#include "Common.h"
#include "GameState.h"
#include "Map.h"
#include "Action.h"
#include "Cost.h"

// Encapsulate Strategy related classes
namespace Strategy {

  // Enum to measure if the game is over
  enum GameStatus {
    InProgress,
    Won,
    Tied
  };

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

      // Add a menu entry to the debug menu
      void addDebugMenuEntries() override;

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

      // Get the decision from the AI controllers
      std::future<std::pair<bool, std::stack<Action>>> aiDecision_;
      bool isAIThinking_ = false;

      // Player pathfinding route
      std::vector<Action> path_;

      // The grid of the playing field to draw
      sf::VertexArray grid_;

      // Line of sights
      std::vector<Coord> lineOfSight_;
      std::vector<std::pair<Coord, Range>> unitsInSight_;

      // Should turns or states be recorded?
      bool isRecordingStates_ = true;

      // Should a unit move or attack
      bool isInAttackMode_;

      // Costs for actions to show player on HUD
      Points mpCost_;
      Points apCost_;

      // Sizes and positions of game graphics
      float maxGameLength_;
      float tileLength_;
      sf::Vector2f center_;
      float left_;
      float top_;
      float right_;
      float bottom_;

      // Buttons for the game
      sf::RectangleShape modeButton_;
      sf::RectangleShape endTurnButton_;

      // Map editor variables
      bool enableEditor_ = false;
      Team editorTeam_ = Team(0);
      Object editorObject_ = Object::Nothing;

      // AI viewer variables
      bool enableAIViewer_ = false;

      ///////////////////////////////////////////
      // PURE FUNCTIONS:
      // - Functions without side effects
      // - Used to transform or read game states
      ///////////////////////////////////////////

      // Estimate the Cost of completing a turn from the current State 
      static Cost heuristic(const GameState& state);

      // Evaluate how good an action is going to be
      static Cost weighAction(
          const GameState& start,
          const GameState& from, 
          const GameState& to,
          const Action& action);

      // Attempt to take action on a gamestate
      static std::pair<bool, GameState> takeAction(
          const GameState& state,
          const Action& action);

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

      // Check to see whether there is anything obstructing a and b
      // Uses Bresenhams line drawing algorithm
      static std::vector<Coord> getLineOfSight(
          const Map& map, 
          const Coord& a,
          const Coord& b);

      // Get enemy units in line of sight
      static std::vector<std::pair<Coord, Range>> getUnitsInSight(
          const Map& map,
          const Coord& location);

      // Check the number of allies in range of enemies and enemies in range of
      // allies in the current state for the given team
      static std::pair<unsigned int, unsigned int> getAlliesAndEnemiesInRange(
          const GameState& state,
          const Team& team);

      // Get possible moves from the current Coord in a state
      static std::vector<Action> getPossibleMoves(const GameState& state);

      // Get all attacks for the selected unit in range
      static std::vector<Action> getPossibleAttacks(const GameState& state);

      // Get all possible actions one could take
      static std::vector<Action> getAllPossibleActions(const GameState& state);

      // Check to see if a State is an endpoint for decision making
      static bool isStateEndpoint(const GameState& a, const GameState& b);

      // Check if there's a winning team and retrieve it if so
      static std::pair<GameStatus, Team> getGameStatus(const GameState& state);

      // Translate coords into map index
      static unsigned int coordToIndex(const Map& m, const Coord& coord);

      // Translate map index into a coord
      static Coord indexToCoord(const Map& m, unsigned int index);

      // Get a default map layout of units
      static Map getDefaultUnitPlacement(const Map& map);

      ///////////////////////////////////////////
      // IMPURE FUNCTIONS:
      // - Mutate the state of the scene
      ///////////////////////////////////////////

      // Recursively push states by querying AI controllers
      void continueGame();

      // Clear future states when things happen
      void clearFutureStates();

      // View the latest state stored
      void viewLatestState();
    
      // Resets the state of tic-tac-toe back to the beginning
      void resetGame();

      // Try to perform an action and push the state if possible
      std::pair<bool, GameState> tryPushAction(
          const GameState& state, 
          const Action& action);

      // Pushes a new state into the state list
      void pushState(const GameState& state);

      // Get a gamestate safely
      std::pair<bool, const GameState> getState(unsigned int n) const;

      // Get the controller for a team
      Controller::Type getController(const Team& team) const;

      // Get the reference to the controller for a team
      Controller::Type& getControllerRef(const Team& team);

      // Calculate the path_ variable from selected to hoveredTile_
      void recalculatePath();

      // Recalculate the line of sight set
      void recalculateLineOfSight();

      // Retrieve units that are in sight
      void recalculateUnitsInSight();

      ///////////////////////////////////////////
      // GRAPHICAL / LOGGING:
      // - Non-logic pure or impure functions
      ///////////////////////////////////////////

      // Log actions to terminal
      void logAction(const GameState& state, const Action& action);

      // Adjust graphics for current game size
      void resizeGame();

      // Render an object in on the field
      void renderObject(
          sf::RenderWindow& window, 
          const Team& team,
          const Object& object,
          const Coord& coords,
          const RenderStyle& style);

      // Render pathfinding on the current state
      void renderPath(
          sf::RenderWindow& window, 
          const GameState& state,
          const Object& object);

      // Render text for the game
      void renderText(
          sf::RenderWindow& window,
          unsigned int size,
          const std::string& text,
          const sf::Vector2f& pos,
          const sf::Color& colour = sf::Color::White);

      // Render the game buttons
      void renderButtons(sf::RenderWindow& window);

      // Render resources
      void renderResources(sf::RenderWindow& window, const GameState& state);

      // Get the colour associated with a team
      static sf::Color getTeamColour(const Team& team);
  };
}

#endif
