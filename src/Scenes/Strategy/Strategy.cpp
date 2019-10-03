// Strategy.cpp
// A strategy game for testing AI

#include "Strategy.h"

///////////////////////////////////////////
// SCENE FUNCTIONS:
// - Mandatory functions for the scene
///////////////////////////////////////////

// When the scene starts set up a game
void 
Strategy::Game::onBegin() {

}

// Update the currently hovered tile
void 
Strategy::Game::onUpdate(const sf::Time& dt) {

}

// Handle input and game size changes
void 
Strategy::Game::onEvent(const sf::Event& event) {

  // Resize game if window changes size
  if (event.type == sf::Event::Resized) {
    resizeGame();
  }
}

// Render the game
void 
Strategy::Game::onRender(sf::RenderWindow& window) {

  // Render the game's grid
  window.draw(grid_);

  // @TODO: Remove this
  // Render an object at (1, 1)
  renderObject(sf::Vector2u(1, 1));
}

// Whenever the scene is re-shown, ensure graphics are correct
void 
Strategy::Game::onShow() {
  resizeGame();
}

// Add details to debug windows
void 
Strategy::Game::addDebugDetails() {

}

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
void
Strategy::Game::resizeGame() {

  // Get display size
  const auto displaySize = App::getDisplaySize();

  // Get square size based on smallest dimension of display
  maxGameLength_ = 
      (displaySize.x < displaySize.y ? 
      displaySize.x : displaySize.y) * 0.75f;

  // Get width and length of battlefield in tiles
  const auto fieldSize = currentState_.size;

  // Use the smallest dimension of playfield to calculate tile size
  tileLength_ = 
    maxGameLength_ / (fieldSize.x < fieldSize.y ?
    fieldSize.x : fieldSize.y);

  // Calculate important positions on screen to draw the game
  center_ = displaySize * 0.5f;
  left_ = center_.x - (fieldSize.x * tileLength_) * 0.5f;
  top_ = center_.y - (fieldSize.y * tileLength_) * 0.5f;
  right_ = left_ + (fieldSize.x * tileLength_);
  bottom_ = top_ + (fieldSize.y * tileLength_);

  // Create a grid out of lines
  const unsigned int lineCount = fieldSize.x + fieldSize.y + 2;
  grid_ = sf::VertexArray(sf::Lines, lineCount * 2);
  unsigned int count = 0;

  // Create horizontal lines
  for (unsigned int j = 0; j <= fieldSize.y; ++j) {
    grid_[count]      = sf::Vector2f(left_, top_ + j * tileLength_);
    grid_[count + 1]  = sf::Vector2f(right_, top_ + j * tileLength_);
    count += 2;
  }
  // Create vertical lines
  for (unsigned int i = 0; i <= fieldSize.x; ++i) {
    grid_[count]      = sf::Vector2f(left_ + i * tileLength_, top_);
    grid_[count + 1]  = sf::Vector2f(left_ + i * tileLength_, bottom_);
    count += 2;
  }
}

// Render an object in on the field
void 
Strategy::Game::renderObject(const sf::Vector2u& coords) {

  // @TODO: Render a wall at desired location for now
}
