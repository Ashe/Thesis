// TicTacToe.cpp
// A scene for testing pathfinding with a game of tic-tac-toe

#include "TicTacToe.h"

// Update the currently hovered tile
// @TODO: Move the tile around
void 
TicTacToeScene::onUpdate(const sf::Time& dt) {

  // Easy out: no tilesize
  if (tileSize_ == 0.f) { mouseTile_ = sf::Vector2i(-1, -1); }

  // Get mouse location relative to top-left of game board
  auto coords = App::getMousePosition() - sf::Vector2f(left_, top_);

  // Scale it down by tileSize_
  coords = sf::Vector2f(coords / tileSize_);

  // Convert coords into tile ints
  auto tile = sf::Vector2i(
      std::floor(coords.x),
      std::floor(coords.y));

  // If tile is valid, set mouseTile_ as it
  if (tile.x >= 0 && tile.x < BOARDSIZE
      && tile.y >= 0 && tile.y < BOARDSIZE) {
    mouseTile_ = tile;
    return;
  }

  // If execution reaches here, invalidate mouseTile_
  mouseTile_ = sf::Vector2i(-1, -1);
}

// Handle input and game size changes
void
TicTacToeScene::onEvent(const sf::Event& event) {

  // If the user clicks
  if (event.type == sf::Event::MouseButtonPressed) {

    // If its left mouse:
    if (event.mouseButton.button == sf::Mouse::Left) {

      // Get the current state for playmaking
      const auto statePair = getState(currentState_);

      // Make the move IF
      // - The state found was valid
      // - It's PlayerX's turn AND they're human OR
      // - It's PlayerO's turn AND they're human
      if (statePair.first
          && ((statePair.second.currentTurn == Player::X
            && playerX == Controller::Human)
          || (statePair.second.currentTurn == Player::O
            && playerO == Controller::Human))) {

        // Get the new state after making a move
        const auto newState = 
            makeMove(statePair.second, mouseTile_.x, mouseTile_.y);

        // If the move was successful
        if (newState.first) {

          // Log successful move
          const auto controllerString = getControllerAsString(
              statePair.second.currentTurn == Player::X ? playerX : playerO);
          Console::log("%d> Player %s (%s) made move: (%d, %d)",
              currentState_,
              statePair.second.currentTurn == Player::X ? "X" : "O",
              controllerString.c_str(),
              mouseTile_.x, mouseTile_.y);

          // Erase future data
          states_.erase(states_.begin() + currentState_ + 1, states_.end());
      
          // Add new state and move currentState_ to the last state
          states_.push_back(newState.second);
          currentState_ = states_.size() - 1;
          if (currentState_ < 0) { currentState_ = 0; }
        }
      }
    }
  }

  // If the window has been resized, handle graphics
  else if (event.type == sf::Event::Resized) {
    resizeGame();
  }
}

// Make a move and get a new state
std::pair<bool, const GameState>
TicTacToeScene::makeMove(const GameState& state, int x, int y) {

  // If its left mouse:
  if (x >= 0 && x < BOARDSIZE && y >= 0 && y < BOARDSIZE) {

    // If the desired tile is unnocupied
    if (state.boardState[y][x] == Player::N) {

      // Duplicate state and make the move
      auto newState = state;
      newState.boardState[y][x] = state.currentTurn;

      // Add a new move
      newState.turnNumber += 1;

      // Alternate who's turn it is
      newState.currentTurn = state.currentTurn == Player::X 
        ? Player::O : Player::X;

      // Return the new state with a success flag
      return std::make_pair(true, newState);
    }
  }

  // Signify that things went wrong
  return std::make_pair(false, GameState());
}

// Render the render the game board and state
void
TicTacToeScene::onRender(sf::RenderWindow& window) {

  // Try to get the current state
  auto statePair = getState(currentState_);

  // If a state was found
  if (statePair.first) {

    // Draw the background first
    window.draw(board_);

    // Draw the state of the game
    drawGameState(window, statePair.second);
  }
}

// Get a gamestate safely
std::pair<bool, const GameState> 
TicTacToeScene::getState(unsigned int n) const {

  // Fetch the correct state if possible
  if (n >= 0 && n < states_.size()) {
    return std::make_pair(true, states_[n]);
  }

  // If nothing found, return invalid pair
  return std::make_pair(false, GameState());
}

// Whenever the scene is re-shown, ensure graphics are correct
void
TicTacToeScene::onShow() {

  // Ensure board sizes are corect
  resizeGame();
}

// Draw the given state
void
TicTacToeScene::drawGameState(
    sf::RenderWindow& window, 
    const GameState& state) {

  // Draw mouse's hovered tile IF
  // - It's a human's turn
  // - There's nothing occupying the current tile
  if (((state.currentTurn == Player::X && playerX == Controller::Human)
        || (state.currentTurn == Player::O && playerO == Controller::Human))
      && mouseTile_.x < 3 && mouseTile_.y < 3
      && mouseTile_.x >= 0 && mouseTile_.y >= 0
      && state.boardState[mouseTile_.y][mouseTile_.x] == Player::N) {

    // Draw the hovered mouse icon
    drawIcon(window, mouseTile_.x, mouseTile_.y, state.currentTurn, true);
  }

  // Draw icons each tile of the board
  for (int j = 0; j < BOARDSIZE; ++j) {
    for (int i = 0; i < BOARDSIZE; ++i) {
      const Player tile = state.boardState[j][i];
      drawIcon(window, i, j, tile);
    }
  }
}

// Add details to info debug windows
void
TicTacToeScene::addDebugDetails() {

  // Retrieve the current state
  const auto statePair = getState(currentState_);
  if (!statePair.first) { return; }
  const auto state = statePair.second;

  ImGui::Begin("State Viewer");
  ImGui::Text("State: %u", currentState_);
  ImGui::PushButtonRepeat(true);
  if (currentState_ > 0) {
    ImGui::SameLine();
    if (ImGui::ArrowButton("##left", ImGuiDir_Left)) {
      --currentState_;
      Console::log("Switched to prev state: %d", currentState_);
    }
  }
  if (currentState_ < states_.size() - 1) {
    ImGui::SameLine();
    if (ImGui::ArrowButton("##right", ImGuiDir_Right)) {
      ++currentState_;
      Console::log("Switched to next state: %d", currentState_);
    }
  }
  ImGui::Text("X Controller: %s", getControllerAsString(playerX).c_str());
  ImGui::Text("O Controller: %s", getControllerAsString(playerO).c_str());
  ImGui::Text("Turn: %u (%s)", 
      state.turnNumber,
      state.currentTurn == Player::X ? "X" : "O");
  ImGui::Text("Next tile: (%d, %d)",
      mouseTile_.x, mouseTile_.y);
  ImGui::End();
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
  playerIconX_[1].position = sf::Vector2f(tileSize_ * 0.8f, tileSize_ * 0.8f);
  playerIconX_[2].position = sf::Vector2f(tileSize_ * 0.8f, tileSize_ * 0.2f);
  playerIconX_[3].position = sf::Vector2f(tileSize_ * 0.2f, tileSize_ * 0.8f);
  // Player O
  playerIconO_ = sf::CircleShape(tileSize_ * 0.35f);
  playerIconO_.setOrigin(tileSize_ * 0.35f, tileSize_ * 0.35f);
  playerIconO_.setOutlineThickness(1.f);
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

// Draw an icon in a tile
void 
TicTacToeScene::drawIcon(
    sf::RenderWindow& window, 
    unsigned int x, 
    unsigned int y, 
    Player player,
    bool hovered) {

  // Grab the player who owns the tile
  const auto pos = sf::Vector2f(
      left_ + x * tileSize_, 
      top_ + y * tileSize_);

  // Select the correct colour based on params
  const sf::Color colour =
      player == Player::X ?
          (hovered ? playerXColourHovered_ : playerXColour_) :
          (hovered ? playerOColourHovered_ : playerOColour_);

  // Draw a cross if appropriate
  if (player == Player::X) {
    auto cross = playerIconX_;
    for (int k = 0; k < 4; ++k) {
      cross[k].position += pos;
      cross[k].color = colour;
    }
    window.draw(cross);
  }

  // Draw a circle if appropriate
  else if (player == Player::O) {
    auto circle = playerIconO_;
    circle.setPosition(pos + sf::Vector2f(tileSize_, tileSize_) * 0.5f);
    circle.setOutlineColor(colour);
    window.draw(circle);
  }
}

// Get a string of the kind of controller
std::string 
TicTacToeScene::getControllerAsString(const Controller& controller) {
  switch(controller) {
    case Controller::Random: return "Random"; break;
    default: return "Human"; break;
  }
}
