// TicTacToe.h
// A scene for testing pathfinding with a game of tic-tac-toe

#ifndef TICTACTOE_H
#define TICTACTOE_H

#include <math.h>
#include <stack>
#include <utility>

#include "../../Scene.h"
#include "../../Controller/Common.h"
#include "../../Controller/Random/Random.h"
#include "../../Controller/AStar/AStar.h"

#include "Common.h"
#include "GameState.h"
#include "Cost.h"

// Encapsulate TicTacToe related classes
namespace TicTacToe {

  // Basic game of tic-tac-toe
  class Game : public Scene {
    public:

      ///////////////////////////////////////////
      // SCENE FUNCTIONS:
      // - Mandatory functions for the scene
      ///////////////////////////////////////////

      // @TODO: Delete this
      void onBegin() override {
        states_.push_back(GameState());
        continueGame();
      }

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

      // Tracker for which state to view
      unsigned int currentState_;

      // States of the game
      std::vector<GameState> states_;

      // Has the game been won at some point?
      bool isGameOver_;
      Player winner_;

      // Who is playing who
      Controller::Type playerX_ = Controller::Type::Human;
      Controller::Type playerO_ = Controller::Type::Human;

      // Player colours
      sf::Color playerXColour_ = sf::Color(0, 117, 252);
      sf::Color playerXColourHovered_ = sf::Color(0, 0, 130);
      sf::Color playerOColour_ = sf::Color(255, 0, 0);
      sf::Color playerOColourHovered_ = sf::Color(130, 0, 0);

      // Player icons
      sf::VertexArray playerIconX_;
      sf::CircleShape playerIconO_;

      // The tic-tac-toe board
      sf::VertexArray board_;

      // The mouse hovered tile
      sf::Vector2i mouseTile_;

      // Key dimensions
      float gameSize_;
      float tileSize_;

      // Key Positions
      sf::Vector2f center_;
      float top_, left_, right_, bottom_;

      ///////////////////////////////////////////
      // PURE FUNCTIONS:
      // - Functions without side effects
      // - Used to transform or read game states
      ///////////////////////////////////////////
      
      // Get a collection of valid moves one could make
      static std::vector<Move> getValidMoves(const GameState& state);

      // Check if the game has been won by a player
      // Returns (isGameOver, winner)
      static std::pair<bool, const Player> checkGameover(
          const GameState& state);

      // Attempts to make the move on the game state and returns new state
      // Returns (isStateValid, newState)
      static std::pair<bool, const GameState> makeMove(
          const GameState& state, 
          const Move& move);

      // Check if a move is valid
      static bool isValidMove(const Move& move);

      // Check to see if a state is a valid destination point
      // Here, only one move can be made, so any valid move could be a goal
      static bool isStateGoal(const GameState& from, const GameState& to);

      // Estimate the cost to get to a suitable destination node
      static Cost estimateCostHeuristic(const GameState& state);

      // Determine the cost of performing a move with given state
      static Cost weighMove(
          const GameState& from, 
          const GameState& to, 
          const Move& move);
    
      ///////////////////////////////////////////
      // IMPURE FUNCTIONS:
      // - Mutate the state of the scene
      ///////////////////////////////////////////
      
      // Recursive function for checking and performing AI moves
      void continueGame();

      // Reset's the state of tic-tac-toe back to the beginning
      void resetGame();

      // Get a gamestate safely
      std::pair<bool, const GameState> getState(unsigned int n) const;

      // Check if a human player can make a move
      bool isGamePlayable();

      // Check which controller is currently playing
      Controller::Type getControllerOfCurrentPlayer(const Player& player) const;

      ///////////////////////////////////////////
      // GRAPHICAL / LOGGING:
      // - Non-logic pure or impure functions
      ///////////////////////////////////////////

      // Adjust graphics for current game size
      void resizeGame();

      // Draw a specific gamestate
      void drawGameState(sf::RenderWindow& window, const GameState& state);

      // Draw an icon in a tile
      void drawIcon(
          sf::RenderWindow& window, 
          Move move,
          Player player,
          bool hovered = false);

      // Log to console the move that was just performed
      void logMove(int stateNo, Player currentTurn, Move move) const;
  };
}

#endif
