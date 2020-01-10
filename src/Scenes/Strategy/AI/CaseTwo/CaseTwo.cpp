// CaseTwo.cpp
// A wrapper containing the A* class

#include "CaseTwo.h"

// Initialise statics
unsigned int Strategy::AI::CaseTwo::endTurnMultiplier = 1;

///////////////////////////////////////////
// AI CONTROLLER FUNCTIONS
///////////////////////////////////////////

// Start making the decision
std::pair<bool, std::stack<Strategy::Action>> 
Strategy::AI::CaseTwo::operator()(const GameState& state) {
  return astar(
      state, 
      minimumCost, 
      maximumCost, 
      Game::getAllPossibleActions,
      isStateEndpoint,
      heuristic,
      weighAction,
      Game::takeAction,
      personality);
}

// Get number of states processed so far
const unsigned int
Strategy::AI::CaseTwo::getStatesProcessed() const {
  return astar.getStatesProcessed();
}

// Get number of open states remaining
const unsigned int
Strategy::AI::CaseTwo::getOpenStatesRemaining() const {
  return astar.getRemaining().size();
}

// Debugging functionality
void
Strategy::AI::CaseTwo::debug() {
  ImGui::Text("Personality tweaking:");
  ImGui::InputFloat("Remaining enemies", 
      &personality.remainingEnemyMultiplier, 0.f, 50.0f, "%.5f");
  ImGui::InputFloat("Lost allies", 
      &personality.lostAlliesMultiplier, 0.f, 50.0f, "%.5f");
  ImGui::InputFloat("Allies at risk", 
      &personality.alliesAtRiskMultiplier, 0.f, 50.0f, "%.5f");
  ImGui::InputFloat("Unused MP", 
      &personality.unusedMPMultiplier, 0.f, 50.0f, "%.5f");
  ImGui::InputFloat("Unused AP", 
      &personality.unusedAPMultiplier, 0.f, 50.0f, "%.5f");

  ImGui::Spacing();
  ImGui::Text("Overall cost multiplier when ending turn:");
  ImGui::InputInt("End turn multiplier", 
      (int*)&endTurnMultiplier, 0, 150);

}


///////////////////////////////////////////
// AI COMPONENTS
///////////////////////////////////////////

// Check to see if a State is an endpoint for decision making
bool 
Strategy::AI::CaseTwo::isStateEndpoint(const GameState& a, const GameState& b) {
  return Game::hasTurnEnded(a, b);
}


// Estimate the Cost of completing a turn from the current State 
Strategy::AI::CaseTwo::Cost 
Strategy::AI::CaseTwo::heuristic(const GameState& state) {
  return minimumCost;
}

// Evaluate how good an action is going to be
Strategy::AI::CaseTwo::Cost 
Strategy::AI::CaseTwo::weighAction(
    const GameState& start,
    const GameState& from, 
    const GameState& to,
    const Action& action) {

  // Prepare to weight the action
  Cost cost = minimumCost;

  // Get the current team
  const Team& team = from.currentTeam;

  // Apply penalties for leaving enemies alive
  // - this will make the AI prefer states with less enemies
  for (const auto& kvp : to.teams) {
    if (kvp.first != team) {
      cost.remainingEnemyPenalty += kvp.second;
    }
  }

  // Check to see if any allies have been lost
  // - This allows the AI to prefer states where they keep allies alive
  // - This is for friendly fire, allies can't die during their turn otherwise
  auto itFrom = from.teams.find(team);
  auto itTo = to.teams.find(team);
  cost.lostAlliesPenalty = 0;
  if (itFrom != from.teams.end()) {

    // If values for both can be found, use the difference to find lost allies
    if (itTo != to.teams.end()) {
      if (itTo->second < itFrom->second) {
        cost.lostAlliesPenalty = itTo->second - itFrom->second;
      }
    }

    // If there's no entry in the current state, all allies have died
    else {
      cost.lostAlliesPenalty = itFrom -> second;
    }
  }

  // Check to see if there are allies within the range of enemies
  // - This will allow the AI to prioritise moving allies out of range / sight
  for (const auto& object : to.map.field) {

    // Only iterate through allied units
    const auto& pos = Game::indexToCoord(to.map, object.first);
    const auto& unit = object.second;
    if (unit.first == team && isUnit(unit.second)) {

      // Check to see if any enemies are in sight
      const auto& enemies = Game::getUnitsInSight(to.map, pos);

      // Check to see if the ally is in range of an enemy
      bool inEnemyRange = false;
      for (unsigned int i = 0; !inEnemyRange && i < enemies.size(); ++i) {
        const auto& enemyAndDistance = enemies[i];
        const auto& object = Game::readMap(to.map, enemyAndDistance.first);
        const auto& range = getUnitRange(object.second);

        // If the ally is in range of an enemy, record it 
        if (enemyAndDistance.second <= range) {
          inEnemyRange = true;
          cost.alliesAtRiskPenalty += 1;
        }
      }
    }
  }

  // If the current action ends the turn, apply a penalty
  if (action.tag == Action::Tag::EndTurn) {
    cost.unusedMPPenalty += from.remainingMP;
    cost.unusedAPPenalty += from.remainingAP;
    cost = cost * endTurnMultiplier;
  }

  // Return the calculated cost of this action
  return cost;
}
