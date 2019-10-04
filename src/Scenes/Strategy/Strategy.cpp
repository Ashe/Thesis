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
  renderObject(window, Object::Bazooka, sf::Vector2u(0, 0));
  renderObject(window, Object::Shotgunner, sf::Vector2u(1, 0));
  renderObject(window, Object::Machinegunner, sf::Vector2u(0, 1));
  renderObject(window, Object::Wall, sf::Vector2u(1, 1));
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
  const auto col = sf::Color::White * sf::Color(255, 255, 255, 50);
  grid_ = sf::VertexArray(sf::Lines, lineCount * 2);

  // Create horizontal lines
  for (unsigned int j = 0; j <= fieldSize.y; ++j) {
    grid_.append(sf::Vertex(
          sf::Vector2f(left_, top_ + j * tileLength_), col));
    grid_.append(sf::Vertex(
          sf::Vector2f(right_, top_ + j * tileLength_), col));
  }
  // Create vertical lines
  for (unsigned int i = 0; i <= fieldSize.x; ++i) {
    grid_.append(sf::Vertex(
          sf::Vector2f(left_ + i * tileLength_, top_), col));
    grid_.append(sf::Vertex(
          sf::Vector2f(left_ + i * tileLength_, bottom_), col));
  }
}

// Render an object in on the field
void 
Strategy::Game::renderObject(
    sf::RenderWindow& window, 
    const Strategy::Object& object,
    const sf::Vector2u& coords) {

  // Declare static variables for rendering easily
  static std::map<Object, sf::Sprite> sprites;

  // Try to find the desired sprite
  const auto it = sprites.find(object);
  if (it != sprites.end()) {

    // Get sprite out of it
    auto& sprite = it->second;

    // Get position from coords
    const auto pos = sf::Vector2f(
        left_ + coords.x * tileLength_,
        top_ + coords.y * tileLength_);

    // Manipulate sprite to draw
    sprite.setPosition(pos);
    const auto texSize = sprite.getTextureRect();
    sprite.setScale(tileLength_ / texSize.width, tileLength_ / texSize.height);
    window.draw(sprite);
  }

  // If sprite wasn't found, try to load it
  else {

    // Prepare to create the new sprite for the required texture
    sf::Sprite sprite;
    sf::Texture* tex = nullptr;

    // Wall texture
    if (object == Object::Wall) {
      tex = App::resources().getTexture("wall");
    }

    // Bazooka user
    else if (object == Object::Bazooka) {
      tex = App::resources().getTexture("bazooka");
    }

    // Machinegunner
    else if (object == Object::Machinegunner) {
      tex = App::resources().getTexture("machinegunner");
    }

    // Shotgunner
    else if (object == Object::Shotgunner) {
      tex = App::resources().getTexture("shotgunner");
    }

    // If a texture was found for this object, store it in the map
    if (tex != nullptr) {
      sprite.setTexture(*tex);
      sprites[object] = sprite;
    }
  }
}
