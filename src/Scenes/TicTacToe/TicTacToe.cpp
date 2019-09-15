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
}

// Adjust graphics for current game size
void
TicTacToeScene::resizeGame() {

  // Get display size
  const auto displaySize = App::getDisplaySize();

  // Get square size based on smallest dimension of display
  const float gameSize = 
      (displaySize.x < displaySize.y ? 
      displaySize.x : displaySize.y) * 0.6f;
  const float tileSize = gameSize / 3.f;

  // Get positions
  const auto center = displaySize * 0.5f;
  const float left = center.x - (gameSize * 0.5f);
  const float top = center.y - (gameSize * 0.5f);
  const float right = left + gameSize;
  const float bottom = top + gameSize;

  // Create a new board
  board_ = sf::VertexArray(sf::Lines, 16);

  // Horizontal lines
  board_[0].position = sf::Vector2f(left, top);
  board_[1].position = sf::Vector2f(right, top);
  board_[2].position = sf::Vector2f(left, top + tileSize);
  board_[3].position = sf::Vector2f(right, top + tileSize);
  board_[4].position = sf::Vector2f(left, top + tileSize * 2);
  board_[5].position = sf::Vector2f(right, top + tileSize * 2);
  board_[6].position = sf::Vector2f(left, bottom);
  board_[7].position = sf::Vector2f(right, bottom);

  // Vertical lines
  board_[8].position = sf::Vector2f(left, top);
  board_[9].position = sf::Vector2f(left, bottom);
  board_[10].position = sf::Vector2f(left + tileSize, top);
  board_[11].position = sf::Vector2f(left + tileSize, bottom);
  board_[12].position = sf::Vector2f(left + tileSize * 2, top);
  board_[13].position = sf::Vector2f(left + tileSize * 2, bottom);
  board_[14].position = sf::Vector2f(right, top);
  board_[15].position = sf::Vector2f(right, bottom);
}
