// TicTacToe.h
// A scene for testing pathfinding with a game of tic-tac-toe

#ifndef TICTACTOE_H
#define TICTACTOE_H

#include "../../Scene.h"

#define BOARDSIZE 3

// Representation of players
enum Player {
  N,
  X,
  O
};

// Simple gamestate structure
struct GameState {

  // Track who's turn it is
  Player currentTurn = Player::X;

  // State of the board
  Player boardState[BOARDSIZE][BOARDSIZE] = {
    { N, N, O },
    { X, X, O },
    { N, N, O }
  };
};

// Basic game of tic-tac-toe
class TicTacToeScene : public Scene {
  public:

    // Whenever the scene is re-shown, ensure graphics are correct
    void onShow() override;

    // Handle input and game size changes
    void onEvent(const sf::Event& event) override;

    // Render the render the game board and state
    void onRender(sf::RenderWindow& window) override;

  private:

    // Player colours
    sf::Color playerXColour_ = sf::Color::Blue;
    sf::Color playerOColour_ = sf::Color::Red;

    // Player icons
    sf::VertexArray playerIconX_;
    sf::CircleShape playerIconO_;

    // The tic-tac-toe board
    sf::VertexArray board_;

    // Key dimensions
    float gameSize_;
    float tileSize_;

    // Key Positions
    sf::Vector2f center_;
    float top_, left_, right_, bottom_;

    // Adjust graphics for current game size
    void resizeGame();

    // Draw a specific gamestate
    void drawGameState(sf::RenderWindow& window, const GameState& state);
};

#endif
