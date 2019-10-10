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

  // Get the current state if possible
  const auto& attempt = getState(currentState_);
  if (!attempt.first) { return; }
  const auto& state = attempt.second;

  // Get the position of the mouse
  const auto& mousePosition = App::getMousePosition();

  // Get the mouse position in game tiles
  const auto hover = Coord(
      static_cast<int>(std::floor((mousePosition.x - left_) / tileLength_)),
      static_cast<int>(std::floor((mousePosition.y - top_) / tileLength_)));

  // Check if hovered tile is valid and set variable accordingly
  const bool valid = validateCoords(currentMap_, hover);
  if (valid) {

    // Check if this is a change in the hovered tile
    const bool changed = hoveredTile_ != hover;
    hoveredTile_ = hover;

    // If this is a new tile
    if (changed) {

      // Recalculate pathfinding route
      recalculatePath();

      // Recalculate line of sight
      recalculateLineOfSight();
    }
  }
  else {
    path_.clear();
    hoveredTile_ = Coord(-1, -1);
  }
}

// Handle input and game size changes
void 
Strategy::Game::onEvent(const sf::Event& event) {

  // Respond to mouse clicks
  if (event.type == sf::Event::MouseButtonPressed) {

    // Get the current state if possible
    const auto& attempt = getState(currentState_);
    if (!attempt.first) { return; }
    const auto& state = attempt.second;

    // Select, move or attack with left click
    if (event.mouseButton.button == sf::Mouse::Left) {

      // Check if the current team is HUMAN and for valid coords
      if (getControllerRef(state.currentTeam) == Controller::Type::Human
          && validateCoords(state.map, hoveredTile_)) {

        // Check to see if we've clicked on a unit
        const auto& entity = readMap(state.map, hoveredTile_);
        if (entity.second >= Object::Bazooka) {

          // Prepare to take action
          Action action;

          // If it's allied select it
          if (entity.first == state.currentTeam) {
            action.tag = Action::Tag::SelectUnit;
            action.location = hoveredTile_;
          }

          // If it's an enemy, attack it (if one is selected)
          else {
            action.tag = Action::Tag::AttackUnit;
            action.location = hoveredTile_;
          }

          // If we selected or attacked, attempt to act
          const auto& newState = takeAction(state, action);
          if (newState.first) {
            pushState(newState.second);
          }
        }
        
        // Otherwise, if it's an empty space, move there if possible
        else if (entity.second == Object::Nothing) {

          // Sample the path as it will get recalculated with pushState()
          std::vector<Action> path;
          for (int i = 0; i < state.remainingMP && i < path_.size(); ++i) {
            path.push_back(path_[i]);
          }

          // Follow set path, pushing and updating state as progress is made
          auto currentState = state;
          for (int i = 0; i < path.size(); ++i) {

            // Get the action from the path
            const auto& action = path[i];
            if (action.tag == Action::Tag::MoveUnit) {
              const auto& newState = takeAction(currentState, action);
              if (newState.first) {
                pushState(newState.second);
                currentState = newState.second;
              }
            }
          }
        }
      }
    }

    // Cancel selection on right click
    else if (event.mouseButton.button == sf::Mouse::Right) {
      Action action;
      action.tag = Action::Tag::CancelSelection;
      const auto& newState = takeAction(state, action);     
      if (newState.first) {
        pushState(newState.second);
      }
    }
  }

  // Resize game if window changes size
  else if (event.type == sf::Event::Resized) {
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

  // Render line of sight with green tiles
  auto rect = sf::RectangleShape(sf::Vector2f(tileLength_, tileLength_));
  rect.setFillColor(sf::Color(255, 0, 0, 50));
  for (const auto& c : lineOfSight_) {

    // Don't re-draw the tile on hover
    if (c != hoveredTile_) {
    
      // Get position from coords
      const auto pos = sf::Vector2f(
          left_ + c.x * tileLength_,
          top_ + c.y * tileLength_);

      // Manipulate rectangle position and draw
      rect.setPosition(pos);
      window.draw(rect);
    }
  }

  // Render everything on the map
  for (const auto& t : map.field) {

    // Retrieve data from the current entry
    const auto& pos = indexToCoord(map, t.first);
    const auto& team = t.second.first;
    const auto& object = t.second.second;

    // Render a tile if they're in sight
    const auto& it = std::find(
        unitsInSight_.begin(), unitsInSight_.end(), pos);
    if (it != unitsInSight_.end()) {
      rect.setPosition(sf::Vector2f(
          left_ + pos.x * tileLength_,
          top_ + pos.y * tileLength_));
      window.draw(rect);
    }

    // Check if this object is selected
    RenderStyle style = team == state.currentTeam ? 
        RenderStyle::Playing : RenderStyle::NotPlaying;

    // Elevate the style if further things apply
    if (team == state.currentTeam 
        && getController(team) == Controller::Type::Human) {
      if (pos == state.selection) { style = RenderStyle::Selected; }
      else if (pos == hoveredTile_) { style = RenderStyle::Hovered; }
    }

    // Render object if coords are valid
    renderObject(window, team, object, pos, style);
  }

  // Render path points
  renderPath(window, state);

  // If the current team is Human and there's something selected
  const auto& currentController = getController(state.currentTeam);
  if (currentController == Controller::Type::Human 
      && validateCoords(map, state.selection)
      && validateCoords(map, hoveredTile_)) {

    // Check to see if the hovered tile is empty
    const auto object = readMap(map, hoveredTile_);
    if (object.second == Object::Nothing) {
      renderObject(window, state.currentTeam, 
          Object::Bazooka, hoveredTile_, RenderStyle::Ghost);
    }
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

  // State viewer
  if (ImGui::Begin("State Viewer")) {
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
    ImGui::Text("Hovered tile: (%d, %d)",
        hoveredTile_.x, 
        hoveredTile_.y);
    ImGui::Text("Selected tile: (%d, %d)",
        state.selection.x, 
        state.selection.y);
    ImGui::Text("Movement points: %d / %d", 
        state.remainingMP, state.map.startingMP);
    ImGui::Text("Action points: %d / %d", 
        state.remainingAP, state.map.startingAP);
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Participating Teams:"); ImGui::NextColumn();
    ImGui::Columns(3, "teamcolumns");
    ImGui::Separator();
    for (const auto& kvp : state.teams) {
      const auto col = getTeamColour(kvp.first);
      ImGui::TextColored(col, "Team %u", kvp.first); 
      ImGui::NextColumn();
      ImGui::TextColored(col, "%u members left", kvp.second); 
      ImGui::NextColumn();
      auto& controller = getControllerRef(kvp.first);
      std::string comboLabel;
      for (unsigned int i = 0; i < kvp.first; ++i) { comboLabel += " "; }
      ImGui::Combo(
          comboLabel.c_str(),
          reinterpret_cast<int*>(&controller), 
          Controller::typeList, IM_ARRAYSIZE(Controller::typeList));
      ImGui::NextColumn();
      ImGui::Separator();
    }
    ImGui::Columns(1);
    ImGui::Spacing();
    ImGui::Text("Current turn: "); ImGui::SameLine();
    const auto teamIt = state.teams.find(state.currentTeam);
    if (teamIt != state.teams.end()) {
      const auto col = getTeamColour(state.currentTeam);
      ImGui::TextColored(col, "Team %u (%u members left)", 
          state.currentTeam, teamIt->second);
    }
    else {
      ImGui::Text("No participating teams to play.");
    }
    if (ImGui::Button("Reset Game")) { resetGame(); }
    ImGui::Spacing();
    if (ImGui::Button(enableEditor_ ? "Hide Editor" : "Show Editor")) {
      enableEditor_ = !enableEditor_;
    }
  }
  ImGui::End();

  // Map editor
  if (enableEditor_) {
    if(ImGui::Begin("Map Editor", &enableEditor_)) {
      ImGui::Text("Map editor here");
    }
    ImGui::End();
  }
}

///////////////////////////////////////////
// PURE FUNCTIONS:
// - Functions without side effects
// - Used to transform or read game states
///////////////////////////////////////////

// Attempt to take action on a gamestate
std::pair<bool, Strategy::GameState>
Strategy::Game::takeAction(const GameState& state, const Action& action) {

  // If the user wishes to undo selection, invalidate selection Coords
  if (action.tag == Action::Tag::CancelSelection) {
    auto newState = state;
    newState.selection = Coord(-1, -1);
    return std::make_pair(true, newState);
  }

  // If the action is to select a unit, attempt to select it
  else if (action.tag == Action::Tag::SelectUnit) {

    // Validate that there is an ALLIED unit to select
    const auto& unit = readMap(state.map, action.location);
    if (unit.first == state.currentTeam && unit.second >= Object::Bazooka) {

      // Update the selected unit in the current state
      auto newState = state;
      newState.selection = action.location;
      return std::make_pair(true, newState);
    }
  }

  // If the action is to move a unit to a location, attempt it
  else if (action.tag == Action::Tag::MoveUnit) {

    // Validate the move
    const auto& unit = readMap(state.map, state.selection);
    const auto& dest = readMap(state.map, action.location);

    // If 
    // - There's enough MP
    // - Unit selected
    // - Selected unit is friendly
    // - Destination is empty
    if (state.remainingMP > 0
        && unit.second >= Object::Bazooka
        && unit.first == state.currentTeam
        && dest.second == Object::Nothing) {

      // Create a new state with the move performed
      auto newState = state;
      auto updateAttempt = updateMap(
          newState.map,
          action.location,
          unit.second,
          unit.first);

      // If the unit is in the new place successfully
      // remove the old unit after duplication
      if (updateAttempt.first) {
        updateAttempt = updateMap(
            updateAttempt.second,
            state.selection,
            Object::Nothing,
            state.currentTeam);

        // If this was also successful, update newState and return
        if (updateAttempt.first) {

          newState.map = updateAttempt.second;
          newState.selection = action.location;
          newState.remainingMP -= 1;
          return std::make_pair(true, newState);
        }
      }
    }
  }

  // If everything fails, return the failure flag
  return std::make_pair(false, state);
}

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

    // If the thing found is a character
    if (kvp.second.second >= Object::Bazooka) {

      // Increment the count for the found team
      const auto& team = kvp.second.first;
      if (teams.find(team) != teams.end()) { teams[team] += 1; }
      else { teams[team] = 1; }
    }
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


// Check to see whether there is anything obstructing a and b
// Uses Bresenhams line drawing algorithm
// From https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
std::vector<Strategy::Coord>
Strategy::Game::getLineOfSight(
    const Map& map, 
    const Coord& from,
    const Coord& to,
    int range) {

  // Should low-line or high-line be used
  bool useHigh;
  Coord start = from;
  Coord end = to;

  // Determine which version of Bresenham's algorithm to use
  if (std::abs(to.y - from.y) < std::abs(to.x - from.x)) {
    useHigh = false;
    if (from.x > to.x) {
      start = to;
      end = from;
    }
  }
  else {
    useHigh = true;
    if (from.y > to.y) {
      start = to;
      end = from;
    }
  }

  // Calculate dx and dy
  int dx = end.x - start.x;
  int dy = end.y - start.y;

  // Pool line of sight in case of success
  std::vector<Coord> line;

  // Calculate low lines
  if (!useHigh) {
    int yi = 1;
    if (dy < 0) {
      yi = -1;
      dy = -dy;
    }
    int d = 2 * dy - dx;
    int y = start.y;
    for (int x = start.x; x <= end.x; ++x) {

      // Check that coord is not destination or empty to return fail
      const auto current = Coord(x, y);
      line.push_back(current);
      if (current != from && current != to) {
        const auto read = readMap(map, current);
        if (read.second != Object::Nothing) {
          return std::vector<Coord>();
        }
      }
      if (d > 0) {
        y += yi;
        d -= 2 * dx;
      }
      d += 2 * dy;
    }
  }

  // Calculate high lines
  else {
    int xi = 1;
    if (dx < 0) {
      xi = -1;
      dx = -dx;
    }
    int d = 2 * dx - dy;
    int x = start.x;
    for (int y = start.y; y <= end.y; ++y) {

      // Check that coord is not destination or empty to return fail
      const auto current = Coord(x, y);
      line.push_back(current);
      if (current != from && current != to) {
        const auto read = readMap(map, current);
        if (read.second != Object::Nothing) {
          return std::vector<Coord>();
        }
      }
      if (d > 0) {
        x += xi;
        d -= 2 * dy;
      }
      d += 2 * dx;
    }
  }

  // Return true if nothing has ruined the line
  return line;
}

// Get possible moves from the current Coord in a state
std::vector<Strategy::Action> 
Strategy::Game::getPossibleMoves(const GameState& state) {

  // Prepare to collect movement actions
  std::vector<Action> actions;

  // The 'selected' Coord is the current position
  if (validateCoords(state.map, state.selection)) {
    
    // You can move up, down, left and right
    const auto possible = std::vector<Coord>
        { Coord(1, 0), Coord(0, -1), Coord(-1, 0), Coord(0, 1) };

    // Check if the moves are valid AND if they're unoccupied
    for (const auto& m : possible) {
      const auto pos = state.selection + m;
      if (validateCoords(state.map, pos)) {
        const auto& entity = readMap(state.map, pos);
        if (entity.second == Object::Nothing) {
          Action action;
          action.tag = Action::Tag::MoveUnit;
          action.location = pos;
          actions.push_back(action);
        }
      }
    }
  }

  // Return possible moves
  return actions;
}

///////////////////////////////////////////
// IMPURE FUNCTIONS:
// - Mutate the state of the scene
///////////////////////////////////////////

// Resets the state of tic-tac-toe back to the beginning
void 
Strategy::Game::resetGame() {

  // Report that we're starting a new game
  Console::log("Game reset.");

  // Clear and re-initialise gamestate
  states_.clear();
  GameState state;
  state.map = currentMap_;
  state.teams = countTeams(state.map);
  const auto it = state.teams.begin();
  state.currentTeam = it != state.teams.end() ? it->first : -1;
  state.remainingMP = state.map.startingMP;
  state.remainingAP = state.map.startingAP;
  states_.push_back(state);
  //isGameOver_ = false;

  // Start the game
  //continueGame();

  // Set current state to the most up-to-date state
  currentState_ = states_.size() - 1;
  if (currentState_ < 0) { currentState_ = 0; }
}

// Pushes a new state into the state list
void 
Strategy::Game::pushState(const GameState& state) {

  // Log the action
  // @TODO: Log action
  
  // Erase future states and start from here
  states_.erase(states_.begin() + currentState_ + 1, states_.end());

  // Add new state and move currentState_ to the last state
  states_.push_back(state);

  // Check for AI interactions on the latest state
  // @TODO: Implement 'continue game' function
  //continueGame();

  // Observe the latest state after AI moves
  currentState_ = states_.size() - 1;
  if (currentState_ < 0) { currentState_ = 0; }

  // Recalculate the path after things have changed
  recalculatePath();

  // Recalculate line of sights
  recalculateLineOfSight();
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

// Get the controller for a team (inserts HUMAN if not found)
Controller::Type
Strategy::Game::getController(const Team& team) const {
  const auto& it = controllers_.find(team);
  if (it != controllers_.end()) {
    return it->second;
  }
  
  // If nothing is found, return Type::Human
  return Controller::Type::Human;
}

// Get the reference to a controller for a team (inserts HUMAN if not found)
Controller::Type&
Strategy::Game::getControllerRef(const Team& team) {
  const auto& it = controllers_.find(team);
  if (it != controllers_.end()) {
    return it->second;
  }
  
  // If nothing is found, insert Type::Human and return that
  controllers_[team] = Controller::Type::Human;
  return getControllerRef(team);
}

// Calculate the path_ variable from selected to hoveredTile_
void 
Strategy::Game::recalculatePath() {

  // Prepare to calculate a new route
  path_.clear();

  // Retrieve the current state
  const auto statePair = getState(currentState_);
  if (!statePair.first) { return; }
  const auto state = statePair.second;

  // Easy out if the hovered coords or selection coords are invalid
  if (!validateCoords(state.map, hoveredTile_)
      || !validateCoords(state.map, state.selection)) {
    return;
  }


  // Make a state with near-infinite movement points for pathfinding
  auto infinState = state;
  infinState.remainingMP = INT_MAX;

  // Employ the use of AStar to find a path
  auto attempt = Controller::AStar::decide
    <GameState, Action, unsigned int>(
      infinState,
      0,
      UINT_MAX,
      getPossibleMoves,

      // IsStateEndpoint (is the tile = to the hovered tile)
      [this](const GameState& a, const GameState& b) {
        return b.selection == hoveredTile_;
      },

      // Heuristic (just do pythagoras to get distance)
      [this](const GameState& s) {
        const auto l = s.selection.x - hoveredTile_.x;
        const auto h = s.selection.y - hoveredTile_.y;
        const auto hyp = std::sqrt(l * l + h * h);
        return static_cast<unsigned int>(std::ceil(hyp));
      },

      // Weigh action (every move is 1 MP)
      [](const GameState& a, const GameState& b, const Action& action) {
        return 1;
      },
      takeAction,
      std::less<unsigned int>()
  );

  // If the path worked
  if (attempt.first) {

    // Unwind stack
    while (!attempt.second.empty()) {
      const auto& action = attempt.second.top();
      path_.push_back(action);
      attempt.second.pop();
    }
  }
}

// Recalculate the line of sight set
void
Strategy::Game::recalculateLineOfSight() {

  // Prepare to calculate sights
  lineOfSight_.clear();
  unitsInSight_.clear();

  // Retrieve the current state
  const auto statePair = getState(currentState_);
  if (!statePair.first) { return; }
  const auto state = statePair.second;

  // Check if selection is valid
  const auto& selection = readMap(state.map, state.selection);
  if (validateCoords(state.map, state.selection) 
      && selection.first == state.currentTeam
      && selection.second >= Object::Bazooka) {

    // Iterate through every enemy and determine if they're in line of sight
    for (const auto& kvp : state.map.field) {
      const auto& pos = indexToCoord(state.map, kvp.first);
      const auto& team = kvp.second.first;
      const auto& object = kvp.second.second;

      // If this is an enemy unit, record if it's in sight or not
      if (team != state.currentTeam && object >= Object::Bazooka) {
        const auto& line = getLineOfSight(
            state.map, state.selection, pos);
        if (!line.empty()) {
          unitsInSight_.push_back(pos);
        }
      }
    }

    // Get line of sight to hovered object IF:
    // - Hovered coords are valid
    // - Hovered object exists
    // - Hovered object isn't allied (unless its a wall)
    const auto hoveredObject = readMap(state.map, hoveredTile_);
    if (validateCoords(state.map, hoveredTile_)
        && hoveredObject.second != Object::Nothing
        && (hoveredObject.first != state.currentTeam || 
            hoveredObject.second == Object::Wall)) {

      // Populate the set with the line of sight
      lineOfSight_ = getLineOfSight(
          state.map,
          state.selection,
          hoveredTile_);
    }
  }
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
    const Coord& coords,
    const RenderStyle& style) {

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

    // Get initial sprite colour
    auto col = getTeamColour(team);

    // Manipulate sprite colour - dim if not highlighted
    switch (style) {
      case RenderStyle::NotPlaying:
        col *= sf::Color(255, 255, 255, 150); break;
      case RenderStyle::Playing:
        col *= sf::Color(255, 255, 255, 200); break;
      case RenderStyle::Hovered:
        col *= sf::Color(255, 255, 255, 255); break;
      case RenderStyle::Selected:
        col = sf::Color(255, 204, 0, 255); break;
      case RenderStyle::Ghost:
      default: col *= sf::Color(255, 255, 255, 175); break;
    };
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

// Render pathfinding on the current state
void 
Strategy::Game::renderPath(sf::RenderWindow& window, const GameState& state) {

  // Make a sprite and initialise it if necessary
  static sf::Sprite pointSprite;
  if (pointSprite.getTexture() == nullptr) {
    auto* tex = App::resources().getTexture("path_point");
    pointSprite.setTexture(*tex);
  }

  // Manipulate sprite position, size and colour
  const auto texSize = pointSprite.getTextureRect();
  pointSprite.setScale(tileLength_ / texSize.width, tileLength_ / texSize.height);

  // Render each point in the path
  for (int i = 0; i < path_.size(); ++i) {

    // Get the current path node if this is definitely a move action
    if (path_[i].tag == Action::Tag::MoveUnit) {
      const auto& p = path_[i].location;

      // Calculate if the player has enough MP to reach this node
      auto col = getTeamColour(state.currentTeam);
      if (i >= state.remainingMP) {
        col *= sf::Color(255, 255, 255, 50);
      }
      pointSprite.setColor(col);

      // Ensure that this location is valid, empty and not the hovered tile
      if (validateCoords(state.map, p) && p != hoveredTile_
          && readMap(state.map, p).second == Object::Nothing) {

        // Get position from coords
        pointSprite.setPosition(sf::Vector2f(
            left_ + p.x * tileLength_,
            top_ + p.y * tileLength_));

        // Draw the point
        window.draw(pointSprite);
      }
    }
  }
}

// Get the colour associated with a team
sf::Color
Strategy::Game::getTeamColour(const Team& team) {
  const auto& coloursIt = teamColours.find(team);
  auto col = sf::Color::White;
  if (coloursIt != teamColours.end()) { 
    col = coloursIt->second; 
  }
  return col;
}
