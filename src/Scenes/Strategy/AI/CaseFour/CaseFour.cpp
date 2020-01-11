// CaseFour.cpp
// A wrapper containing the A* class

#include "CaseFour.h"

// Static initialisations
typedef Strategy::AI::CaseFour::Cost CaseFourCost;
unsigned int CaseFourCost::Penalty::optionalActionPenalty = 1;
unsigned int CaseFourCost::Penalty::selectUnit = 3;
unsigned int CaseFourCost::Penalty::spentMP = 1;
unsigned int CaseFourCost::Penalty::spentAP = 2;
unsigned int CaseFourCost::Penalty::turnEnded = 2;
unsigned int CaseFourCost::Penalty::attackedNothing = 20;
unsigned int CaseFourCost::Penalty::attackedFriendly = 20;

unsigned int CaseFourCost::Predictions::allyNeedsSaving = 2;
unsigned int CaseFourCost::Predictions::alliesFurtherExposed = 4;
unsigned int CaseFourCost::Predictions::enemyNeedsEliminating = 10;
unsigned int CaseFourCost::Predictions::enemyNeedsExposing = 2;
unsigned int CaseFourCost::Predictions::needToMoveCloser = 2;

bool Strategy::AI::CaseFour::enableGoalMoveOrKill = true;

///////////////////////////////////////////
// AI CONTROLLER FUNCTIONS
///////////////////////////////////////////

// Start making the decision
std::pair<bool, std::stack<Strategy::Action>> 
Strategy::AI::CaseFour::operator()(const GameState& state) {
  return astar(
      state, 
      minimumCost, 
      maximumCost, 
      Game::getAllPossibleActions,
      isStateEndpoint,
      HeuristicFunctor(state, enableGoalMoveOrKill),
      weighAction,
      Game::takeAction,
      std::less<Cost>());
}

// Get number of states processed so far
const unsigned int
Strategy::AI::CaseFour::getStatesProcessed() const {
  return astar.getStatesProcessed();
}

// Get number of open states remaining
const unsigned int
Strategy::AI::CaseFour::getOpenStatesRemaining() const {
  return astar.getRemaining().size();
}

// Debugging functionality
void
Strategy::AI::CaseFour::debug() {
  const auto& actionAndCost = astar.getCurrentAction();
  ImGui::Columns(2);
  ImGui::Text("%s (%d, %d)",
      actionToString(actionAndCost.first),
      actionAndCost.first.location.x,
      actionAndCost.first.location.y);
  ImGui::NextColumn();
  ImGui::Text("Cost: %u",
      actionAndCost.second.value);
  ImGui::Columns(1);
  Cost totalCost = minimumCost;
  const auto& fScores = astar.getFScores();
  for (const auto& kvp : fScores) {
    totalCost = totalCost + kvp.second;
  }
  ImGui::Text("Average cost: %f", 
      (float)totalCost.value / fScores.size());
  ImGui::Spacing(); ImGui::Spacing();
  ImGui::PushItemWidth(30.f);
  ImGui::Text("Goal customisation:");
  ImGui::Checkbox("Enable goal 'eliminate one unit or move closer'", 
      &enableGoalMoveOrKill);
  ImGui::Text("Penalty customisation:");
  ImGui::Text("Remember, most of these are applied at the end of a turn.");
  ImGui::InputInt("Action cost", 
      (int*)&Cost::Penalty::optionalActionPenalty, 0, 30);
  ImGui::InputInt("Select unit", 
      (int*)&Cost::Penalty::selectUnit, 0, 30);
  ImGui::InputInt("Spent MP", 
      (int*)&Cost::Penalty::spentMP, 0, 30);
  ImGui::InputInt("Spent AP", 
      (int*)&Cost::Penalty::spentAP, 0, 30);
  ImGui::InputInt("End turn", 
      (int*)&Cost::Penalty::turnEnded, 0, 30);
  ImGui::InputInt("Attacked nothing", 
      (int*)&Cost::Penalty::attackedNothing, 0, 30);
  ImGui::InputInt("Attacked friendly", 
      (int*)&Cost::Penalty::attackedFriendly, 0, 30);
  ImGui::InputInt("Ally needs saving", 
      (int*)&Cost::Predictions::allyNeedsSaving, 0, 30);
  ImGui::InputInt("Allies further exposed", 
      (int*)&Cost::Predictions::alliesFurtherExposed, 0, 30);
  ImGui::InputInt("Enemy needs eliminating", 
      (int*)&Cost::Predictions::enemyNeedsEliminating, 0, 30);
  ImGui::InputInt("Enemy needs exposing", 
      (int*)&Cost::Predictions::enemyNeedsExposing, 0, 30);
  ImGui::InputInt("Need to move closer", 
      (int*)&Cost::Predictions::needToMoveCloser, 0, 30);
  ImGui::PopItemWidth();
}

///////////////////////////////////////////
// AI COMPONENTS
///////////////////////////////////////////

// Check to see if a State is an endpoint for decision making
// A is the starting state, B is the current state
bool 
Strategy::AI::CaseFour::isStateEndpoint(
    const GameState& a, const GameState& b) {

  // If the turn hasn't been ended, it can't be a goal
  if (!Game::hasTurnEnded(a, b)) {
    return false;
  }

  // If the goal is to move closer
  if (enableGoalMoveOrKill) {

    // Count allies and enemies
    const auto& prevCounts = Game::countTeams(a.map);
    unsigned int previousEnemyCount = 0;

    // Sum the counts of the other teams into the enemy count
    for (const auto& kvp : prevCounts) {
      if (kvp.first != a.currentTeam) {
        previousEnemyCount += kvp.second;
      }
    }

    // Count allies and enemies
    const auto& counts = Game::countTeams(b.map);
    unsigned int enemyCount = 0;

    // Sum the counts of the other teams into the enemy count
    for (const auto& kvp : counts) {
      if (kvp.first != a.currentTeam) {
        enemyCount += kvp.second;
      }
    }

    // If an enemy has been killed, it's a goal
    if (enemyCount < previousEnemyCount) {
      return true;
    }

    // Otherwise, check average distances
    float previousAverageDistanceToEnemies = 0.f;
    for (const auto& e : a.map.field) {
      const auto& pos = Game::indexToCoord(a.map, e.first);
      auto comps = sf::Vector2f(
        abs((int)a.selection.x - (int)pos.x),
        abs((int)a.selection.y - (int)pos.y));
      comps.x *= comps.x;
      comps.y *= comps.y;
      const auto dist = sqrt(comps.x + comps.y);
      previousAverageDistanceToEnemies += dist;
    }

    float currentAverageDistanceToEnemies = 0.f;
    for (const auto& e : b.map.field) {
      const auto& pos = Game::indexToCoord(b.map, e.first);
      auto comps = sf::Vector2f(
        abs((int)b.selection.x - (int)pos.x),
        abs((int)b.selection.y - (int)pos.y));
      comps.x *= comps.x;
      comps.y *= comps.y;
      const auto dist = sqrt(comps.x + comps.y);
      currentAverageDistanceToEnemies += dist;
    }

    // Whether this is a goal depends if the team has moved closer
    return currentAverageDistanceToEnemies < previousAverageDistanceToEnemies;
  }

  // Return true if no goals restrictions enabled
  return true;
}

// Constructor for heuristic functor
Strategy::AI::CaseFour::HeuristicFunctor::HeuristicFunctor
    (const GameState& state, bool& goal) : startingState(state), useGoal(goal) {

  // Count allies and enemies
  const auto& counts = Game::countTeams(state.map);
  allyCount = 0;
  enemyCount = 0;

  // Iterate through teams
  for (const auto& kvp : counts) {

    // If they're allies, use the value for ally count
    if (kvp.first == state.currentTeam) {
      alliesInRange = kvp.second;
    }

    // Sum the counts of the other teams into the enemy count
    else {
      enemyCount += kvp.second;
    }
  }

  // Prepare to check allies and enemies in range
  const auto& inRange = Game::getAlliesAndEnemiesInRange(
      startingState, startingState.currentTeam);
  alliesInRange = inRange.first;
  enemiesInRange = inRange.second;
}

// Estimate the Cost of completing a turn from the current State 
Strategy::AI::CaseFour::Cost 
Strategy::AI::CaseFour::HeuristicFunctor::operator()(const GameState& state) {

  // Turn hasn't ended, so prepare to calculate heuristic
  Cost cost = minimumCost;

  // Has the turn been ended? 
  // - Predict the end turn penalty 
  // - Return instantaneously, as this is a goal
  if (!Game::hasTurnEnded(startingState, state)) {

    // If the game is in progress, has a character been selected? 
    // - Predict that a character will need to be chosen to reach the goal
    if (state.selection == Coord(-1, -1)) {
      cost.value += Cost::Penalty::selectUnit;
    }
  }

  // Have allies been made less exposed through attacking or moving?
  // - Predict that some sort of movement will need to be made
  const auto& inRange = Game::getAlliesAndEnemiesInRange(
      state, startingState.currentTeam);

  // Are there allies that need to be secured?
  // - Predict that each exposed ally needs saving
  cost.value += inRange.first * Cost::Predictions::allyNeedsSaving;

  // If there are more allies exposed than previously, add penalty
  if (inRange.first > alliesInRange) {
    cost.value += Cost::Predictions::alliesFurtherExposed;
  }

  // Are there enemies that need eliminating?
  // - Predict that each enemy needs killing
  cost.value += inRange.second * Cost::Predictions::enemyNeedsEliminating;

  // Are there hidden enemies that need exposing?
  // - Predict that each unexposed enemy needs exposing
  if (enemyCount > inRange.second) {
    cost.value += (enemyCount - inRange.second) * 
        Cost::Predictions::enemyNeedsExposing;
  }

  // If the goal is in use, penalise based on closest distance from enemies
  if (useGoal) {

    // Get the current closest distance
    float d = Game::getDistanceToClosestEnemy(
        state.map, startingState.currentTeam);
    cost.value += floor(d) * Cost::Predictions::needToMoveCloser;
  }

  // Return the heuristic cost of reaching the goal from this state
  return cost;
}

// Evaluate how good an action is going to be
Strategy::AI::CaseFour::Cost 
Strategy::AI::CaseFour::weighAction(
    const GameState& start,
    const GameState& from, 
    const GameState& to,
    const Action& action) {

  // Prepare to calculate the cost of this action
  Cost cost = minimumCost;

  // Apply optional forced penalty
  cost.value += Cost::Penalty::optionalActionPenalty;

  // Flat penalty for selecting units
  if (action.tag == Action::Tag::SelectUnit
      || action.tag == Action::Tag::CancelSelection) {
    cost.value += Cost::Penalty::selectUnit;
  }

  // Apply resources spent on movement as a penalty
  else if (action.tag == Action::MoveUnit) {
    cost.value += 
        (from.remainingMP - to.remainingMP) * Cost::Penalty::spentMP;
  }

  // Apply resources spent on attacking as a penalty
  else if (action.tag == Action::Tag::Attack) {

    // Equate the cost of the action to the resources spent
    cost.value += 
        (from.remainingAP - to.remainingAP) * Cost::Penalty::spentAP;

    // Did this attack do anything useful?
    // - Apply a penalty for not hitting anything
    // - Apply a penalty for attacking friendlies
    const auto& hit = Game::readMap(from.map, action.location);

    // Check for hitting nothing
    if (hit.second == Object::Nothing) {
      cost.value += Cost::Penalty::attackedNothing;
    }

    // Check for hitting allies
    else if (hit.second != Object::Wall && hit.first == from.currentTeam) {
      cost.value += Cost::Penalty::attackedFriendly;
    }
  }

  // Apply a cost to ending the turn
  else if (action.tag == Action::Tag::EndTurn) {

    // Cost for ending the turn
    cost.value += Cost::Penalty::turnEnded;

    // 'Spend' the remaining MP and AP
    cost.value += from.remainingMP * Cost::Penalty::spentMP
        + from.remainingAP * Cost::Penalty::spentAP;
  }

  // Return the calculated cost of this action
  return cost;
}
