// CaseFour.cpp
// A wrapper containing the A* class

#include "CaseFour.h"

///////////////////////////////////////////
// AI CONTROLLER FUNCTIONS
///////////////////////////////////////////

// Start making the decision
std::pair<bool, std::stack<Strategy::Action>> 
Strategy::AI::CaseFour::operator()(const GameState& state) {

  // Count allies and enemies
  const auto& counts = Game::countTeams(state.map);
  startingAlliesInRange = 0;
  startingEnemiesInRange = 0;

  // Iterate through teams
  for (const auto& kvp : counts) {

    // If they're allies, use the value for ally count
    if (kvp.first == state.currentTeam) {
      startingAlliesInRange = kvp.second;
    }

    // Sum the counts of the other teams into the enemy count
    else {
      startingEnemiesInRange += kvp.second;
    }
  }

  // Prepare to check allies and enemies in range
  const auto& inRange = Game::getAlliesAndEnemiesInRange(
      startingState, startingState.currentTeam);
  startingAllyCount = inRange.first;
  startingEnemyCount = inRange.second;

  // Perform decision
  return astar(
      state, 
      minimumCost, 
      maximumCost, 
      Game::getAllPossibleActions,
      std::bind(&CaseFour::isStateEndpoint, this,
        std::placeholders::_1,
        std::placeholders::_2),
      std::bind(&CaseFour::heuristic, this,
        std::placeholders::_1),
      std::bind(&CaseFour::weighAction, this,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4),
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
      (int*)&penalties.optionalActionPenalty, 0, 30);
  ImGui::InputInt("Select unit", 
      (int*)&penalties.selectUnit, 0, 30);
  ImGui::InputInt("Spent MP", 
      (int*)&penalties.spentMP, 0, 30);
  ImGui::InputInt("Spent AP", 
      (int*)&penalties.spentAP, 0, 30);
  ImGui::InputInt("End turn", 
      (int*)&penalties.turnEnded, 0, 30);
  ImGui::InputInt("Attacked nothing", 
      (int*)&penalties.attackedNothing, 0, 30);
  ImGui::InputInt("Attacked wall", 
      (int*)&penalties.attackedWall, 0, 30);
  ImGui::InputInt("Attacked friendly", 
      (int*)&penalties.attackedFriendly, 0, 30);
  ImGui::InputInt("Ally needs saving", 
      (int*)&predictions.allyNeedsSaving, 0, 30);
  ImGui::InputInt("Allies further exposed", 
      (int*)&predictions.alliesFurtherExposed, 0, 30);
  ImGui::InputInt("Enemy needs eliminating", 
      (int*)&predictions.enemyNeedsEliminating, 0, 30);
  ImGui::InputInt("Enemy needs exposing", 
      (int*)&predictions.enemyNeedsExposing, 0, 30);
  ImGui::InputInt("Need to move closer", 
      (int*)&predictions.needToMoveCloser, 0, 30);
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

    // Whether this is a goal depends if the team has moved closer
    const float currentDistanceToClosestEnemy = 
        Game::getDistanceToClosestEnemy( b.map, a.currentTeam);
    return currentDistanceToClosestEnemy < startingDistanceToClosestEnemy;
  }

  // Return true if no goals restrictions enabled
  return true;
}

// Estimate the Cost of completing a turn from the current State 
Strategy::AI::CaseFour::Cost 
Strategy::AI::CaseFour::heuristic(const GameState& state) {

  // Turn hasn't ended, so prepare to calculate heuristic
  Cost cost = minimumCost;

  // Has the turn been ended? 
  // - Predict the end turn penalty 
  // - Return instantaneously, as this is a goal
  if (!Game::hasTurnEnded(startingState, state)) {

    // If the game is in progress, has a character been selected? 
    // - Predict that a character will need to be chosen to reach the goal
    if (state.selection == Coord(-1, -1)) {
      cost.value += penalties.selectUnit;
    }
  }

  // Have allies been made less exposed through attacking or moving?
  // - Predict that some sort of movement will need to be made
  const auto& inRange = Game::getAlliesAndEnemiesInRange(
      state, startingState.currentTeam);

  // Are there allies that need to be secured?
  // - Predict that each exposed ally needs saving
  cost.value += inRange.first * predictions.allyNeedsSaving;

  // If there are more allies exposed than previously, add penalty
  if (inRange.first > startingAlliesInRange) {
    cost.value += predictions.alliesFurtherExposed;
  }

  // Are there enemies that need eliminating?
  // - Predict that each enemy needs killing
  cost.value += inRange.second * predictions.enemyNeedsEliminating;

  // Are there hidden enemies that need exposing?
  // - Predict that each unexposed enemy needs exposing
  if (startingEnemyCount > inRange.second) {
    cost.value += (startingEnemyCount - inRange.second) * 
        predictions.enemyNeedsExposing;
  }

  // If the goal is in use, penalise based on closest distance from enemies
  if (enableGoalMoveOrKill) {

    // Get the current closest distance
    float d = Game::getDistanceToClosestEnemy(
        state.map, startingState.currentTeam);
    cost.value += floor(d) * predictions.needToMoveCloser;
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
  cost.value += penalties.optionalActionPenalty;

  // Flat penalty for selecting units
  if (action.tag == Action::Tag::SelectUnit
      || action.tag == Action::Tag::CancelSelection) {
    cost.value += penalties.selectUnit;
  }

  // Apply resources spent on movement as a penalty
  else if (action.tag == Action::MoveUnit) {
    cost.value += 
        (from.remainingMP - to.remainingMP) * penalties.spentMP;
  }

  // Apply resources spent on attacking as a penalty
  else if (action.tag == Action::Tag::Attack) {

    // Equate the cost of the action to the resources spent
    cost.value += 
        (from.remainingAP - to.remainingAP) * penalties.spentAP;

    // Did this attack do anything useful?
    // - Apply a penalty for not hitting anything
    // - Apply a penalty for attacking friendlies
    const auto& hit = Game::readMap(from.map, action.location);

    // Check for hitting nothing
    if (hit.second == Object::Nothing) {
      cost.value += penalties.attackedNothing;
    }

    // Check for hitting a wall
    else if (hit.second == Object::Wall) {
      cost.value += penalties.attackedWall;
    }

    // Check for hitting allies
    else if (hit.first == from.currentTeam) {
      cost.value += penalties.attackedFriendly;
    }
  }

  // Apply a cost to ending the turn
  else if (action.tag == Action::Tag::EndTurn) {

    // Cost for ending the turn
    cost.value += penalties.turnEnded;

    // 'Spend' the remaining MP and AP
    cost.value += from.remainingMP * penalties.spentMP
        + from.remainingAP * penalties.spentAP;
  }

  // Return the calculated cost of this action
  return cost;
}
