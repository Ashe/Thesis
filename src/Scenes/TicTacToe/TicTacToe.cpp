// TicTacToe.cpp
// A scene for testing pathfinding with a game of tic-tac-toe

#include "TicTacToe.h"

// Whenever the scene is re-shown, ensure graphics are correct
void
TicTacToeScene::onShow() {

  // Ensure board sizes are corect
  resizeGame();
}

// Handle input and game size changes
void
TicTacToeScene::onEvent(const sf::Event& event) {

  // If the window has been resized, handle graphics
  if (event.type == sf::Event::Resized) {
    resizeGame();
  }
}

// Render the render the game board and state
void
TicTacToeScene::onRender(sf::RenderWindow& window) {

  // Draw the background first
  window.draw(board_);

  // Draw the state of the game
  // @TODO: Load the gamestate rather than making the fresh on
  drawGameState(window, GameState());
}

// Draw the given state
void
TicTacToeScene::drawGameState(
    sf::RenderWindow& window, 
    const GameState& state) {

  // For each tile of the board
  for (int j = 0; j < BOARDSIZE; ++j) {
    for (int i = 0; i < BOARDSIZE; ++i) {

      // Grab the player who owns the tile
      const Player tile = state.boardState[j][i];
      const auto pos = sf::Vector2f(
          left_ + i * tileSize_, 
          top_ + j * tileSize_);
      
      // Only act if a player owns the tile
      if (tile == Player::X) {
        auto cross = playerIconX_;
        for (int k = 0; k < 4; ++k) {
          cross[k].position += pos;
        }
        window.draw(cross);
      }
      else if (tile == Player::O) {
        auto circle = playerIconO_;
        circle.setPosition(pos + sf::Vector2f(tileSize_, tileSize_) * 0.5f);
        window.draw(circle);
      }
    }
  }
}

// Adjust graphics for current game size
void
TicTacToeScene::resizeGame() {

  // Get display size
  const auto displaySize = App::getDisplaySize();

  // Get square size based on smallest dimension of display
  gameSize_ = 
      (displaySize.x < displaySize.y ? 
      displaySize.x : displaySize.y) * 0.6f;
  tileSize_ = gameSize_ / (float) BOARDSIZE;

  // Get positions
  center_ = displaySize * 0.5f;
  left_ = center_.x - (gameSize_ * 0.5f);
  top_ = center_.y - (gameSize_ * 0.5f);
  right_= left_ + gameSize_;
  bottom_ = top_ + gameSize_;

  // Create player symbols
  // Player X
  playerIconX_ = sf::VertexArray(sf::Lines, 4);
  playerIconX_[0].position = sf::Vector2f(tileSize_ * 0.2f, tileSize_ * 0.2f);
  playerIconX_[0].color = playerXColour_;
  playerIconX_[1].position = sf::Vector2f(tileSize_ * 0.8f, tileSize_ * 0.8f);
  playerIconX_[1].color = playerXColour_;
  playerIconX_[2].position = sf::Vector2f(tileSize_ * 0.8f, tileSize_ * 0.2f);
  playerIconX_[2].color = playerXColour_;
  playerIconX_[3].position = sf::Vector2f(tileSize_ * 0.2f, tileSize_ * 0.8f);
  playerIconX_[3].color = playerXColour_;
  // Player O
  playerIconO_ = sf::CircleShape(tileSize_ * 0.35f);
  playerIconO_.setOrigin(tileSize_ * 0.35f, tileSize_ * 0.35f);
  playerIconO_.setOutlineThickness(1.f);
  playerIconO_.setOutlineColor(playerOColour_);
  playerIconO_.setFillColor(sf::Color::Transparent);

  // Create a board to draw
  board_ = sf::VertexArray(sf::Lines, 16);
  // Horizontal lines
  board_[0].position = sf::Vector2f(left_, top_);
  board_[1].position = sf::Vector2f(right_, top_);
  board_[2].position = sf::Vector2f(left_, top_ + tileSize_);
  board_[3].position = sf::Vector2f(right_, top_ + tileSize_);
  board_[4].position = sf::Vector2f(left_, top_ + tileSize_ * 2);
  board_[5].position = sf::Vector2f(right_, top_ + tileSize_ * 2);
  board_[6].position = sf::Vector2f(left_, bottom_);
  board_[7].position = sf::Vector2f(right_, bottom_);
  // Vertical lines
  board_[8].position = sf::Vector2f(left_, top_);
  board_[9].position = sf::Vector2f(left_, bottom_);
  board_[10].position = sf::Vector2f(left_ + tileSize_, top_);
  board_[11].position = sf::Vector2f(left_ + tileSize_, bottom_);
  board_[12].position = sf::Vector2f(left_ + tileSize_ * 2, top_);
  board_[13].position = sf::Vector2f(left_ + tileSize_ * 2, bottom_);
  board_[14].position = sf::Vector2f(right_, top_);
  board_[15].position = sf::Vector2f(right_, bottom_);
}
