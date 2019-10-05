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

  // Make a temporary initial map
  // @TODO: Delete this
  auto temp = GameState();
  auto attempt = updateMap(
      temp.map, 
      Coord(0, temp.map.size.y - 1),
      Object::Bazooka,
      0);
  attempt = updateMap(
      attempt.second,
      Coord(temp.map.size.x - 1, 0),
      Object::Bazooka,
      1);
  
  // Load the first map to play with
  currentMap_ = attempt.second;

  // Reset the game properly
  resetGame();
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

  // Get the current state if possible
  const auto& attempt = getState(currentState_);
  if (!attempt.first) { return; }
  const auto& state = attempt.second;

  // Render the game's map
  const auto& map = state.map;

  // Render everything on the map
  for (const auto& t : map.field) {

    // Retrieve data from the current entry
    const auto& pos = indexToCoord(map, t.first);
    const auto& team = t.second.first;
    const auto& object = t.second.second;

    // Render object if coords are valid
    renderObject(window, team, object, pos);
  }
}

// Whenever the scene is re-shown, ensure graphics are correct
void 
Strategy::Game::onShow() {
  resizeGame();
}

// Add details to debug windows
void 
Strategy::Game::addDebugDetails() {

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
  ImGui::Columns(2, "teamcolumns");
  ImGui::Separator();
  ImGui::Text("Team"); ImGui::NextColumn();
  ImGui::Text("Members left"); ImGui::NextColumn();
  ImGui::Separator();
  for (const auto& kvp : state.teams) {
    ImGui::Text("%u", kvp.first); ImGui::NextColumn();
    ImGui::Text("%u", kvp.second); ImGui::NextColumn();
    ImGui::Separator();
  }
  ImGui::Columns(1);
  if (ImGui::Button("Reset")) { resetGame(); }
  ImGui::End();
}

///////////////////////////////////////////
// PURE FUNCTIONS:
// - Functions without side effects
// - Used to transform or read game states
///////////////////////////////////////////

// Translate coords into map index
unsigned int 
Strategy::Game::coordToIndex(const Map& m, const Coord& coord) {
  return coord.x + coord.y * m.size.x;
}

// Translate map index into a coord
Strategy::Coord 
Strategy::Game::indexToCoord(const Map& m, unsigned int index) {
  const unsigned int rem = index % m.size.x;
  return Coord( rem, (index - rem) / m.size.x);
}

// Check if coordinates are valid
bool 
Strategy::Game::validateCoords(const Map& map, const Coord& coords) {
  return coords.x >= 0 && coords.x < map.size.x &&
      coords.y >= 0 && coords.y < map.size.y;
}

// Collect the participating teams
std::map<Strategy::Team, unsigned int> 
Strategy::Game::countTeams(const Map& map) {

  // Iterate through things on the map
  std::map<Team, unsigned int> teams;
  for (const auto& kvp : map.field) {

    // Increment the count for the found team
    const auto& team = kvp.second.first;
    if (teams.find(team) != teams.end()) { teams[team] += 1; }
    else { teams[team] = 1; }
  }

  // Return the map of team counts
  return teams;
}

// Get an object on the play field
std::pair<Strategy::Team, Strategy::Object> 
Strategy::Game::readMap(
    const Map& m, 
    const Coord& pos) {
  const unsigned int index = coordToIndex(m, pos);
  const auto it = m.field.find(index);
  if (it != m.field.end()) {
    return it->second;
  }
  return std::make_pair(0, Object::Nothing);
}

// Update the map in some way
std::pair<bool, Strategy::Map> 
Strategy::Game::updateMap(
    const Map& m, 
    const Coord& pos, 
    const Object& obj, 
    const Team& team) {

  // Easy out if the coordinate isn't valid
  Map map = m;
  if (!validateCoords(m, pos)) { return std::make_pair(false, map); }

  // Find the desired element
  const unsigned int index = coordToIndex(m, pos);
  const auto it = map.field.find(index);

  // If the object is 'nothing', this is a call to delete
  if (obj == Object::Nothing) {
    if (it != map.field.end()) {
      map.field.erase(it);
    }
  }

  // Otherwise, insert new data into the map
  else {
    map.field[index] = std::make_pair(team, obj);
  }

  // Return new Map
  return std::make_pair(true, map);
}

///////////////////////////////////////////
// IMPURE FUNCTIONS:
// - Mutate the state of the scene
///////////////////////////////////////////

// Resets the state of tic-tac-toe back to the beginning
void 
Strategy::Game::resetGame() {

  // Clear and re-initialise gamestate
  states_.clear();
  GameState state;
  state.map = currentMap_;
  state.teams = countTeams(state.map);
  states_.push_back(state);
  //isGameOver_ = false;

  // Start the game
  //continueGame();

  // Set current state to the most up-to-date state
  currentState_ = states_.size() - 1;
  if (currentState_ < 0) { currentState_ = 0; }
}

// Get a gamestate safely
std::pair<bool, const Strategy::GameState> 
Strategy::Game::getState(unsigned int n) const {

  // Fetch the correct state if possible
  if (n >= 0 && n < states_.size()) {
    return std::make_pair(true, states_[n]);
  }

  // If nothing found, return invalid pair
  return std::make_pair(false, GameState());
}


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

  // Get the current state if possible
  const auto& attempt = getState(currentState_);
  if (!attempt.first) { return; }
  const auto& state = attempt.second;

  // Get width and length of battlefield in tiles
  const auto fieldSize = state.map.size;

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
    const Team& team,
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

    // Manipulate sprite position and size
    sprite.setPosition(pos);
    const auto texSize = sprite.getTextureRect();
    sprite.setScale(tileLength_ / texSize.width, tileLength_ / texSize.height);

    // Manipulate sprite colour
    const auto it = teamColours.find(team);
    auto col = sf::Color::White;
    if (it != teamColours.end()) {
      col = it->second;
    }
    sprite.setColor(col);

    // Draw sprite
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
