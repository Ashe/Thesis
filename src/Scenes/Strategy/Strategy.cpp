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
  auto attempt = updateMap(temp.map, 
      Coord(0, 4),
      Object::LaserUnit, 0);

  attempt = updateMap(attempt.second,
      Coord(1, 4),
      Object::BlasterUnit, 0);

  attempt = updateMap(attempt.second,
      Coord(0, 3),
      Object::SniperUnit, 0);

  attempt = updateMap(attempt.second,
      Coord(1, 3),
      Object::MeleeUnit, 0);

  attempt = updateMap(attempt.second,
      Coord(4, 0),
      Object::LaserUnit, 1);

  attempt = updateMap(attempt.second,
      Coord(3, 0),
      Object::BlasterUnit, 1);

  attempt = updateMap(attempt.second,
      Coord(4, 1),
      Object::SniperUnit, 1);

  attempt = updateMap(attempt.second,
      Coord(3, 1),
      Object::MeleeUnit, 1);
  
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

  // Change colour of buttons
  endTurnButton_.setFillColor(
      endTurnButton_.getGlobalBounds().contains(mousePosition) ?
        sf::Color(255, 150, 150, 100) :
        sf::Color(255, 100, 100, 100));
  modeButton_.setFillColor(isInAttackMode_ ?
      (modeButton_.getGlobalBounds().contains(mousePosition) ?
        sf::Color(255, 50, 50, 100) :
        sf::Color(255, 0, 0, 100)) :
      (modeButton_.getGlobalBounds().contains(mousePosition) ?
        sf::Color(150, 150, 255, 100) :
        sf::Color(100, 100, 255, 100)));

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

    // Check if the current team is HUMAN
    if (getControllerRef(state.currentTeam) == Controller::Type::Human) {

      // If the click was in the game
      if (validateCoords(state.map, hoveredTile_)) {

        // Check to see if we've clicked on a unit
        const auto& entity = readMap(state.map, hoveredTile_);

        // Select / deselect on left click
        if (event.mouseButton.button == sf::Mouse::Left) {

          // Prepare to select or deselect
          Action action;
          action.location = hoveredTile_;

          // Select if we clicked a different allied unit
          if (entity.first == state.currentTeam 
              && hoveredTile_ != state.selection
              && isUnit(entity.second)) {
            action.tag = Action::Tag::SelectUnit;
          }

          // Deselect otherwise
          else { 
            action.tag = Action::Tag::CancelSelection; 
          }

          // Perform selection / deselection
          const auto& newState = takeAction(state, action);     
          if (newState.first) {
            pushState(newState.second);
          }
        }

        // Select, move or attack with right click
        else if (event.mouseButton.button == sf::Mouse::Right) {

          // Attack anything if we're in attack mode
          if (isInAttackMode_) {

            // If we're in attack mode, attack wherever was clicked
            Action action;
            action.tag = Action::Tag::Attack;
            action.location = hoveredTile_;
            const auto& newState = takeAction(state, action);
            if (newState.first) {
              pushState(newState.second);
            }
          }

          else {

            // Otherwise, if we're not attacking and we clicked nothing, move
            if (entity.second == Object::Nothing) {

              // Sample the path as it will get recalculated with pushState()
              std::vector<Action> path;
              for (int i = 0; i < state.remainingMP && i < path_.size(); ++i) {
                path.push_back(path_[i]);
              }

              // Follow set path, pushing and updating state as progress is made
              auto currentState = state;
              bool success = true;
              for (int i = 0; success && i < path.size(); ++i) {

                // Get the action from the path
                const auto& action = path[i];
                if (action.tag == Action::Tag::MoveUnit) {
                  const auto& newState = takeAction(currentState, action);
                  if (newState.first) {
                    pushState(newState.second);
                    currentState = newState.second;
                  }
                  else {
                    success = false;
                  }
                }
              }
            }
          }
        }
      }

      // Check if the click is on the mode button
      else if (modeButton_.getGlobalBounds().contains(
          App::getMousePosition())) {
        isInAttackMode_ = !isInAttackMode_;
      }

      // Check if the click is on the end turn button
      else if (endTurnButton_.getGlobalBounds().contains(
          App::getMousePosition())) {
        // @TODO: Implement turn ending
      }

      // Otherwise deselect unit
      else {
        Action action;
        action.tag = Action::Tag::CancelSelection;
        action.location = hoveredTile_;
        const auto& newState = takeAction(state, action);
        if (newState.first) {
          pushState(newState.second);
        }
      }
    }
  }

  // Check for key presses
  else if (event.type == sf::Event::KeyPressed) {

    // Set attack mode on shift key press
    if (event.key.code == sf::Keyboard::LShift) {
      isInAttackMode_ = true;
    }

  }

  // Check for key releases
  else if (event.type == sf::Event::KeyReleased) {

    // Undo attack mode on shift key release
    if (event.key.code == sf::Keyboard::LShift) {
      isInAttackMode_ = false;
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

  // Render game buttons when it's a human's turn
  if (getController(state.currentTeam) == Controller::Type::Human) {
    renderButtons(window);
  }

  // Render MP and AP counters
  renderResources(window, state);

  // Render line of sight with red tiles if in attack mode
  auto rect = sf::RectangleShape(sf::Vector2f(tileLength_, tileLength_));
  if (isInAttackMode_) {
    rect.setFillColor(sf::Color(255, 0, 0, 75));
    for (const auto& c : lineOfSight_) {

      // Get position from coords
      const auto pos = sf::Vector2f(
          left_ + c.x * tileLength_,
          top_ + c.y * tileLength_);

      // Manipulate rectangle position and draw
      rect.setPosition(pos);
      window.draw(rect);
    }
  }

  // Dim the colour for enemies that have you in sight
  // When combined with the above, it'll glow more if you can see them
  rect.setFillColor(sf::Color(255, 0, 0, 35));

  // Render everything on the map
  const auto& map = state.map;
  for (const auto& t : map.field) {

    // Retrieve data from the current entry
    const auto& pos = indexToCoord(map, t.first);
    const auto& team = t.second.first;
    const auto& object = t.second.second;

    // Check to see if the current object is in sight
    const auto& it = std::find_if(
        unitsInSight_.begin(), 
        unitsInSight_.end(), 
        [pos](const auto& u) {
          return u.first == pos;
        });

    // Render a tile if they're in sight
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

  // If the current team is Human and there's something selected
  const auto& currentController = getController(state.currentTeam);
  if (currentController == Controller::Type::Human 
      && validateCoords(map, state.selection)
      && validateCoords(map, hoveredTile_)) {

    // Read map for data on selection and hover
    const auto& selectedUnit = readMap(state.map, state.selection);
    const auto& hoveredObject = readMap(map, hoveredTile_);
    
    // IF:
    // - We're in move move
    // - Hovered tile is empty
    // - Selection is a unit
    // - Selection unit is an ally
    if (!isInAttackMode_
        && hoveredObject.second == Object::Nothing
        && isUnit(selectedUnit.second)
        && selectedUnit.first == state.currentTeam) {

      // Render path points
      renderPath(window, state, selectedUnit.second);

      // Render a ghost of the object to move
      renderObject(window, state.currentTeam, 
          selectedUnit.second, hoveredTile_, RenderStyle::Ghost);
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
    ImGui::Text((mpCost_ > 0 ? 
        "Movement points: %d / %d (- %d)"
        : "Movement points: %d / %d"),
        state.remainingMP, state.map.startingMP, mpCost_);
    ImGui::Text((apCost_ > 0 ?
        "Action points: %d / %d (- %d)"
        : "Action points: %d / %d"),
        state.remainingAP, state.map.startingAP, apCost_);
    ImGui::Text("Attacking: %s", isInAttackMode_ ? "true" : "false");
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
    if (unit.first == state.currentTeam && isUnit(unit.second)) {

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
    // - Unit selected
    // - There's enough MP
    // - Selected unit is friendly
    // - Destination is empty
    if (isUnit(unit.second)
        && state.remainingMP >= getUnitMPCost(unit.second)
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
          newState.remainingMP -= getUnitMPCost(unit.second);
          return std::make_pair(true, newState);
        }
      }
    }
  }

  // If the action is to attack a location
  else if (action.tag == Action::Tag::Attack) {

    // Ensure there is a selected and enemy unit
    const auto& unit = readMap(state.map, state.selection);

    // If:
    // - There is a unit selected
    // - Selected unit is friendly
    // - There's enough AP for the unit to commence an attack
    if (isUnit(unit.second) 
        && state.remainingAP >= getUnitAPCost(unit.second)
        && unit.first == state.currentTeam
        && state.remainingAP ) {

      // Check if the location is in range of the unit
      const auto& line = getLineOfSight(
          state.map, state.selection, action.location);
      if (!line.empty() && line.size() <= getUnitRange(unit.second) + 1) {
        
        // Delete whatever is at the location
        // @NOTE: This is vague to remain future-proof. There are checks
        // performed before this to force you to only attack enemies anyway.
        auto attempt = updateMap(
            state.map, 
            action.location, 
            Object::Nothing, 
            state.currentTeam);

        // If update was successful, make a new state and return it
        if (attempt.first) {
          auto newState = state;
          newState.map = attempt.second;
          newState.teams = countTeams(newState.map);
          newState.remainingAP -= getUnitAPCost(unit.second);
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
    if (isUnit(kvp.second.second)) {

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
    const Coord& to) {

  // Should low-line or high-line be used
  bool useHigh;
  bool swapped = false;
  Coord start = from;
  Coord end = to;

  // Determine which version of Bresenham's algorithm to use
  if (std::abs(to.y - from.y) < std::abs(to.x - from.x)) {
    useHigh = false;
    if (from.x > to.x) {
      swapped = true;
      start = to;
      end = from;
    }
  }
  else {
    useHigh = true;
    if (from.y > to.y) {
      swapped = true;
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

      // Add to line of sight if it's not the starting location
      const auto current = Coord(x, y);
      line.push_back(current);

      // Check that coord is not destination or empty to return fail
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

      // Check that coord is not destination or empty to return fail
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
  if (swapped) {
    std::reverse(line.begin(), line.end());
  }
  return line;
}

// Get units in line of sight
std::vector<std::pair<Strategy::Coord, Strategy::Range>> 
Strategy::Game::getUnitsInSight(const Map& map, const Coord& u) {

  // Prepare to collect units in range
  std::vector<std::pair<Coord, Range>> units;

  // Check if there's a unit at the given position
  const auto& location = readMap(map, u);
  if (validateCoords(map, u) 
      && isUnit(location.second)) {

    // Iterate through every enemy and determine if they're in line of sight
    for (const auto& kvp : map.field) {
      const auto& pos = indexToCoord(map, kvp.first);
      const auto& team = kvp.second.first;
      const auto& object = kvp.second.second;

      // If this is an enemy unit, record if it's in sight or not
      if (team != location.first && isUnit(object)) {
        const auto& line = getLineOfSight(
            map, u, pos);
        if (!line.empty()) {
          const Range r = std::max(0, (int)line.size() - 1);
          units.push_back(std::make_pair(pos, r));
        }
      }
    }
  }

  // Return what was discovered
  return units;
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

  isInAttackMode_ = false;
  recalculatePath();
  recalculateLineOfSight();
  unitsInSight_.clear();
  mpCost_ = Points(0);
  apCost_ = Points(0);
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

  // Recalculate line of sight as selected unit could have changed
  recalculateLineOfSight();

  // Find units in sight if selection is valid
  unitsInSight_.clear();
  if (validateCoords(state.map, state.selection)) {
    unitsInSight_ = getUnitsInSight(state.map, state.selection);
  }
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
  mpCost_ = Points(0);

  // Retrieve the current state
  const auto statePair = getState(currentState_);
  if (!statePair.first) { return; }
  const auto state = statePair.second;

  // Easy out if the hovered coords or selection coords are invalid
  if (!validateCoords(state.map, hoveredTile_)
      || !validateCoords(state.map, state.selection)) {
    return;
  }

  // Get the selected unit and exit if its nonexistant or not allied
  const auto& unit = readMap(state.map, state.selection);
  if (!isUnit(unit.second) || unit.first != state.currentTeam) {
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

    // Unwind stack and accumilate the path's cost
    const auto unitMPCost = getUnitMPCost(unit.second);
    while (!attempt.second.empty()) {
      const auto& action = attempt.second.top();
      path_.push_back(action);
      // @TODO: Take environment into account here
      mpCost_ += unitMPCost;
      attempt.second.pop();
    }
  }
}

// Recalculate the line of sight set
void
Strategy::Game::recalculateLineOfSight() {

  // Prepare to calculate sights
  apCost_ = Points(0);
  lineOfSight_.clear();

  // Retrieve the current state
  const auto statePair = getState(currentState_);
  if (!statePair.first) { return; }
  const auto state = statePair.second;

  // Check if selection is valid
  const auto& selection = readMap(state.map, state.selection);
  if (validateCoords(state.map, state.selection) 
      && selection.first == state.currentTeam
      && isUnit(selection.second)) {

    // Get line of sight to hovered location IF:
    // - Selected unit has enough AP
    // - Hovered coords are valid
    const auto hoveredObject = readMap(state.map, hoveredTile_);
    if (getUnitAPCost(selection.second)
        && validateCoords(state.map, hoveredTile_)) {

      // Retrieve line of sight
      const auto& line = getLineOfSight(
          state.map,
          state.selection,
          hoveredTile_);

      // Limit LoS to the range of selected unit
      // @NOTE: 1 of the tiles in LoS is the current unit, so +1 to range
      const auto& range = getUnitRange(selection.second);
      for (int i = 0; i < range + 1 && i < line.size(); ++i) {
        lineOfSight_.push_back(line[i]);
      }

      // If the unit is in line of sight AND in range, calculate AP cost
      if (!line.empty() && line.size() == lineOfSight_.size()) {
        apCost_ = getUnitAPCost(selection.second);
      }
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

  // Create buttons for the game
  const float height = 50.f;
  modeButton_ = sf::RectangleShape(
      sf::Vector2f((center_.x - left_), height));
  endTurnButton_ = sf::RectangleShape(
      sf::Vector2f((center_.x - left_), height));

  // Position buttons
  modeButton_.setPosition(left_, top_ - height);
  endTurnButton_.setPosition(center_.x, top_ - height);
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

    // Melee unit
    else if (object == Object::MeleeUnit) {
      tex = App::resources().getTexture("melee_unit");
    }

    // Blaster unit
    else if (object == Object::BlasterUnit) {
      tex = App::resources().getTexture("blaster_unit");
    }

    // Sniper unit
    else if (object == Object::SniperUnit) {
      tex = App::resources().getTexture("sniper_unit");
    }

    // Laser unit
    else if (object == Object::LaserUnit) {
      tex = App::resources().getTexture("laser_unit");
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
Strategy::Game::renderPath(
    sf::RenderWindow& window, 
    const GameState& state,
    const Object& object) {

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
  const auto unitCost = getUnitMPCost(object);
  auto cost = Points(0);
  for (int i = 0; i < path_.size(); ++i) {

    // Get the current path node if this is definitely a move action
    if (path_[i].tag == Action::Tag::MoveUnit) {
      const auto& p = path_[i].location;

      // Accumilate the MP cost of moving
      // @TODO: Factor in environment here
      cost += unitCost;

      // Set colour depending on whether they have enough MP
      auto col = getTeamColour(state.currentTeam);
      if (cost > state.remainingMP) {
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

// Render text for the game
void 
Strategy::Game::renderText(
    sf::RenderWindow& window,
    unsigned int size,
    const std::string& str,
    const sf::Vector2f& pos,
    const sf::Color& colour) {
  const auto* font = App::resources().getFont("cabin_font");
  if (font != nullptr) {
    auto text = sf::Text(str, *font, size);
    text.setFillColor(colour);
    const auto bounds = text.getLocalBounds();
    text.setOrigin(bounds.width * 0.5f, bounds.height);
    text.setPosition(pos);
    window.draw(text);
  }
}

// Render text for the game
void 
Strategy::Game::renderButtons(sf::RenderWindow& window) {

  // Draw buttons
  window.draw(modeButton_);
  window.draw(endTurnButton_);

  // Draw button text
  auto pos = modeButton_.getPosition();
  auto bounds = modeButton_.getGlobalBounds();
  renderText(window, 32, 
      isInAttackMode_ ? "Attack" : "Move",
      sf::Vector2f(
          pos.x + bounds.width * 0.5f,
          pos.y + bounds.height * 0.5f));
  pos = endTurnButton_.getPosition();
  bounds = endTurnButton_.getGlobalBounds();
  renderText(window, 32, 
      "End Turn",
      sf::Vector2f(
          pos.x + bounds.width * 0.5f,
          pos.y + bounds.height * 0.5f));
}

// Render resources
void 
Strategy::Game::renderResources(
    sf::RenderWindow& window, 
    const GameState& state) {

  // Render MP and AP counters
  std::string str = "MP: " + std::to_string(state.remainingMP);
  if (!isInAttackMode_ && mpCost_ > 0) {
    str += " (- " + std::to_string(mpCost_) + ")";
  }
  renderText(window, 32, str,
      sf::Vector2f(
          (center_.x - left_) * 0.5f + left_,
          bottom_ + 50),
          !isInAttackMode_ && state.remainingMP - mpCost_ < 0 ?
              sf::Color(255, 0, 0, 255) :
              sf::Color(100, 100, 255, 150));
  str = "AP: " + std::to_string(state.remainingAP);
  if (isInAttackMode_ && apCost_ > 0) {
    str += " (- " + std::to_string(apCost_) + ")";
  }
  renderText(window, 32, str,
      sf::Vector2f(
          (right_ - center_.x) * 0.5f + center_.x,
          bottom_ + 50),
          isInAttackMode_ && state.remainingAP - apCost_ < 0 ?
              sf::Color(255, 0, 0, 255) :
              sf::Color(255, 0, 0, 150));
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
