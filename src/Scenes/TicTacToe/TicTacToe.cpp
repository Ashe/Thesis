// TicTacToe.cpp
// A scene for testing pathfinding with a game of tic-tac-toe

#include "TicTacToe.h"

// Static variables
const TicTacToe::Player GameState::firstPlayer = TicTacToe::Player::X;

///////////////////////////////////////////
// SCENE FUNCTIONS:
// - Mandatory functions for the scene
///////////////////////////////////////////

// Update the currently hovered tile
void 
TicTacToe::Game::onUpdate(const sf::Time& dt) {

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
TicTacToe::Game::onEvent(const sf::Event& event) {

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
      if (statePair.first && 
          getControllerOfCurrentPlayer(statePair.second.currentTurn) 
              == Controller::Type::Human) {

        // Get the new state after making a move
        const auto newState = 
            makeMove(statePair.second, Move(mouseTile_));

        // If the move was successful
        if (newState.first) {

          // Log successful move
          logMove(currentState_, statePair.second.currentTurn, mouseTile_);

          // Erase future data
          states_.erase(states_.begin() + currentState_ + 1, states_.end());
      
          // Add new state and move currentState_ to the last state
          states_.push_back(newState.second);

          // Check for AI interactions on the latest state
          continueGame();

          // Observe the latest state after AI moves
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

// Render the render the game board and state
void
TicTacToe::Game::onRender(sf::RenderWindow& window) {

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

// Whenever the scene is re-shown, ensure graphics are correct
void
TicTacToe::Game::onShow() {

  // Ensure board sizes are corect
  resizeGame();
}

// Add details to info debug windows
void
TicTacToe::Game::addDebugDetails() {

  // Retrieve the current state
  const auto statePair = getState(currentState_);
  if (!statePair.first) { return; }
  const auto state = statePair.second;

  ImGui::Begin("State Viewer");
  ImGui::Text("State: %u", currentState_);
  ImGui::PushButtonRepeat(true);
  ImGui::SameLine();
  if (ImGui::ArrowButton("##left", ImGuiDir_Left)) {
    if (currentState_ > 0) {
      --currentState_;
      Console::log("Switched to prev state: %d", currentState_);
    }
  }
  ImGui::SameLine();
  if (ImGui::ArrowButton("##right", ImGuiDir_Right)) {
    if (currentState_ < states_.size() - 1) {
      ++currentState_;
      Console::log("Switched to next state: %d", currentState_);
    }
  }

  ImGui::Text("X Controller: ");
  ImGui::SameLine();
  ImGui::Combo("", 
      reinterpret_cast<int*>(&playerX_), 
      Controller::typeList, IM_ARRAYSIZE(Controller::typeList));
  ImGui::Text("O Controller: ");
  ImGui::SameLine();
  ImGui::Combo(" ", 
      reinterpret_cast<int*>(&playerO_), 
      Controller::typeList, IM_ARRAYSIZE(Controller::typeList));

  ImGui::Text("Turn: %u (%s)", 
      state.turnNumber,
      state.currentTurn == Player::X ? "X" : "O");
  ImGui::Text("Hovered tile: (%d, %d)",
      mouseTile_.x, mouseTile_.y);
  if(isGameOver_) {
    if(winner_ != Player::N) {
      ImGui::Text("Game over! Winner is player: %s.", 
          TicTacToe::playerToString(winner_).c_str());
    }
    else {
      ImGui::Text("Game over! It's a tie.");
    }
  }
  if (ImGui::Button("Reset")) { resetGame(); }
  ImGui::End();
}

///////////////////////////////////////////
// PURE FUNCTIONS:
// - Functions without side effects
// - Used to transform or read game state
///////////////////////////////////////////

// Get a collection of valid moves one could make
std::vector<TicTacToe::Move>
TicTacToe::Game::getValidMoves(const GameState& state) {

  // Prepare to collect moves from the state
  std::vector<Move> moves;

  // Iterate through the board and collect any tiles that aren't occupied
  const auto board = state.boardState;
  for (int j = 0; j < BOARDSIZE; ++j) {
    for (int i = 0; i < BOARDSIZE; ++i) {
      const auto tile = board[j][i];
      if (tile == Player::N) {
        moves.push_back(Move(i, j));
      }
    }
  }

  // Return findings
  return moves;
}

// Check if the game has been won by a player
// Returns (isGameOver, winner)
std::pair<bool, const TicTacToe::Player>
TicTacToe::Game::checkGameover(const GameState& state) {

  // Prepare to inspect the board
  const auto b = state.boardState;

  // Check row victories (player occupied, all three elements in row are equal)
  for (int j = 0; j < BOARDSIZE; ++j) {
    if (b[j][0] != Player::N && b[j][0] == b[j][1] && b[j][1] == b[j][2]) {
      return std::make_pair(true, b[j][0]);
    }
  }

  // Check col victories (player occupied, all three elements in row are equal)
  for (int i = 0; i < BOARDSIZE; ++i) {
    if (b[0][i] != Player::N && b[0][i] == b[1][i] && b[1][i] == b[2][i]) {
      return std::make_pair(true, b[0][i]);
    }
  }

  // Check for \ diagonal
  if (b[0][0] != Player::N && b[0][0] == b[1][1] && b[1][1] == b[2][2]) {
    return std::make_pair(true, b[0][0]);
  }

  // Check for / diagonal
  if (b[2][0] != Player::N && b[2][0] == b[1][1] && b[1][1] == b[0][2]) {
    return std::make_pair(true, b[2][0]);
  }

  // Check to see if there are still empty spaces
  // If there are, use false to flag that the game is still in progress
  for (int j = 0; j < BOARDSIZE; ++j) {
    for (int i = 0; i < BOARDSIZE; ++i) {
      if (b[j][i] == Player::N) {
        return std::make_pair(false, Player::N);
      }
    }
  }

  // If execution reaches here, board is full but no winner
  return std::make_pair(true, Player::N);
}

// Attempts to make the move on the game state and returns new state
// Returns (isStateValid, newState)
std::pair<bool, const GameState>
TicTacToe::Game::makeMove(const GameState& state, const Move& move) {

  // Extract data from move
  const int x = move.x;
  const int y = move.y;

  // If its left mouse:
  if (x >= 0 && x < BOARDSIZE && y >= 0 && y < BOARDSIZE) {

    // If the desired tile is unnocupied
    if (state.boardState[y][x] == Player::N) {

      // Duplicate state and make the move
      auto newState = state;
      newState.boardState[y][x] = state.currentTurn;

      // Alternate who's turn it is
      newState.currentTurn = state.currentTurn == Player::X 
        ? Player::O : Player::X;

      // If we have cycled back to the first player, increment turn number
      if (newState.currentTurn == GameState::firstPlayer) {
        newState.turnNumber += 1;
      }

      // Return the new state with a success flag
      return std::make_pair(true, newState);
    }
  }

  // Signify that things went wrong
  return std::make_pair(false, GameState());
}

// Check if a move is valid
bool 
TicTacToe::Game::isValidMove(const Move& move) {
  return move.x >= 0 && move.x < BOARDSIZE && 
      move.y >= 0 && move.y < BOARDSIZE;
}

///////////////////////////////////////////
// IMPURE FUNCTIONS:
// - Mutate the state of the scene
///////////////////////////////////////////

// Recursive function for checking and performing AI moves
void
TicTacToe::Game::continueGame() {

  // Get the latest game state
  const unsigned int stateCount = states_.size();
  if (stateCount <= 0) { return; }
  const unsigned int stateNo = stateCount - 1;
  const auto state = states_[stateNo];
  isGameOver_ = false;

  // Check if the game is over
  const auto result = checkGameover(state);
  if (result.first) {
    isGameOver_ = true;
    winner_ = result.second;
    Console::log("Game Over. %s",
        winner_ == Player::N ?
          "It's a tie!" :
          std::string("Winner is player: " + playerToString(winner_)).c_str());
    return;
  }

  // Check what controller is currently playing
  const auto controller = getControllerOfCurrentPlayer(state.currentTurn);
  auto attempt = std::make_pair(false, GameState());

  // If it's a HUMAN turn, do nothing
  if (controller == Controller::Type::Human) {
    return;
  }

  // If its a random, invoke RandomController::Type::decide
  else if (controller == Controller::Type::Random) {
    attempt = Controller::Random::decide<GameState, Move> (
        state, getValidMoves, makeMove);
  }

  // If it's an AStar, invoke AStarController::Type::decide
  else if (controller == Controller::Type::AStar) {
    Console::log("[Error] AStar not yet implemented.");
    //attempt = AStarController::Type::decide<GameState, Move, COST>(
    //    state,
    //    LOWESTCOST,
    //    HIGHESTCOST,
    //    getValidMoves,
    //    ISSTATEGOAL,
    //    HEURISTIC,
    //    WEIGHACTION,
    //    makeMove);
  }

  // If AI successfully made their move, update and continue game
  if (attempt.first) {

    // Log move
    //logMove(stateNo, state.currentTurn, move);

    // Update state collection
    states_.push_back(attempt.second);

    // Continue the game in case AI is next
    continueGame();
    return;
  }
}

// Reset the game back to initial state
void
TicTacToe::Game::resetGame() {

  // Clear and re-initialise gamestate
  states_.clear();
  states_.push_back(GameState());
  isGameOver_ = false;

  // Start the game
  continueGame();

  // Set current state to the most up-to-date state
  currentState_ = states_.size() - 1;
  if (currentState_ < 0) { currentState_ = 0; }
}

// Get a gamestate safely
std::pair<bool, const GameState> 
TicTacToe::Game::getState(unsigned int n) const {

  // Fetch the correct state if possible
  if (n >= 0 && n < states_.size()) {
    return std::make_pair(true, states_[n]);
  }

  // If nothing found, return invalid pair
  return std::make_pair(false, GameState());
}

// Check which controller is currently playing
Controller::Type
TicTacToe::Game::getControllerOfCurrentPlayer(const Player& player) const {
  if (player == Player::X) { return playerX_; }
  else if (player == Player::O) { return playerO_; }
  Console::log("[Error] Couldn't get controller of current player.");
  return Controller::Type::Human;
}

///////////////////////////////////////////
// GRAPHICAL / LOGGING:
// - Non-logic pure or impure functions
///////////////////////////////////////////

// Adjust graphics for current game size
void
TicTacToe::Game::resizeGame() {

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

// Draw the given state
void
TicTacToe::Game::drawGameState(
    sf::RenderWindow& window, 
    const GameState& state) {

  // Draw mouse's hovered tile IF
  // - It's a human's turn
  // - There's nothing occupying the current tile
  if (((state.currentTurn == Player::X && playerX_== Controller::Type::Human)
        || (state.currentTurn == Player::O && playerO_== Controller::Type::Human))
      && mouseTile_.x < 3 && mouseTile_.y < 3
      && mouseTile_.x >= 0 && mouseTile_.y >= 0
      && state.boardState[mouseTile_.y][mouseTile_.x] == Player::N) {

    // Draw the hovered mouse icon
    drawIcon(window, Move(mouseTile_), state.currentTurn, true);
  }

  // Draw icons each tile of the board
  for (int j = 0; j < BOARDSIZE; ++j) {
    for (int i = 0; i < BOARDSIZE; ++i) {
      const Player tile = state.boardState[j][i];
      drawIcon(window, Move(i, j), tile);
    }
  }
}

// Draw an icon in a tile
void 
TicTacToe::Game::drawIcon(
    sf::RenderWindow& window, 
    Move move,
    Player player,
    bool hovered) {

  // Grab the player who owns the tile
  const auto pos = sf::Vector2f(
      left_ + move.x * tileSize_, 
      top_ + move.y * tileSize_);

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

// Log to console the move that was just performed
void 
TicTacToe::Game::logMove(int stateNo, Player currentTurn, Move move) const {
  const auto controllerString = Controller::typeToString(
      currentTurn == Player::X ? playerX_: playerO_);
  Console::log("%d> Player %s (%s) made move: (%d, %d)",
      stateNo,
      currentTurn == Player::X ? "X" : "O",
      controllerString.c_str(),
      move.x, move.y);
}
