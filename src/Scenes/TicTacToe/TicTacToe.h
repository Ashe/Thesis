// TicTacToe.h
// A scene for testing pathfinding with a game of tic-tac-toe

#ifndef TICTACTOE_H
#define TICTACTOE_H

#include <math.h>
#include <utility>
#include "../../Scene.h"

#define BOARDSIZE 3

// Representation of who can play
// @TODO: Factor this out so that other scenes can use it
enum Controller {
  Human,
  Random
};

// Representation of players
enum Player {
  N,
  X,
  O
};

// Simple gamestate structure
struct GameState {

  // Turn number
  unsigned int turnNumber = 0;

  // Track who's turn it is
  Player currentTurn = Player::X;

  // State of the board
  Player boardState[BOARDSIZE][BOARDSIZE] = {
    { N, N, N },
    { N, N, N },
    { N, N, N }
  };
};

// Basic game of tic-tac-toe
class TicTacToeScene : public Scene {
  public:

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

    // Who is playing who
    Controller playerX = Controller::Human;
    Controller playerO = Controller::Random;

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

    // Recursive function for checking and performing AI moves
    void continueGame();

    // Make a move and get a new state
    std::pair<bool, const GameState> makeMove(
        const GameState& state, 
        int x, 
        int y);

    // Get a gamestate safely
    std::pair<bool, const GameState> getState(unsigned int n) const;

    // Adjust graphics for current game size
    void resizeGame();

    // Draw a specific gamestate
    void drawGameState(sf::RenderWindow& window, const GameState& state);

    // Draw an icon in a tile
    void drawIcon(
        sf::RenderWindow& window, 
        unsigned int x, 
        unsigned int y, 
        Player player,
        bool hovered = false);

    // Log to console the move that was just performed
    void logMove(int stateNo, Player currentTurn, int x, int y) const;

    // Get a string of the kind of controller
    std::string getControllerAsString(const Controller& controller) const;
};

#endif
