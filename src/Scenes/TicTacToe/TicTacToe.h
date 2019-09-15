// TicTacToe.h
// A scene for testing pathfinding with a game of tic-tac-toe

#ifndef TICTACTOE_H
#define TICTACTOE_H

#include "../../Scene.h"

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

    // The tic-tac-toe board
    sf::VertexArray board_;

    // Adjust graphics for current game size
    void resizeGame();
};

#endif
