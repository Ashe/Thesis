// CaseFour.cpp
// A wrapper containing the A* class

#include "CaseFour.h"

// Static initialisations
unsigned int Strategy::AI::CaseFour::Cost::Penalty::characterChoice = 1;
unsigned int Strategy::AI::CaseFour::Cost::Penalty::unusedMP = 5;
unsigned int Strategy::AI::CaseFour::Cost::Penalty::unusedAP = 10;
unsigned int Strategy::AI::CaseFour::Cost::Penalty::friendlyFire = 25;
unsigned int Strategy::AI::CaseFour::Cost::Penalty::missShot = 25;
unsigned int Strategy::AI::CaseFour::Cost::Penalty::exposedToEnemy = 5;
unsigned int Strategy::AI::CaseFour::Cost::Penalty::unnecessaryRisk = 5;
unsigned int Strategy::AI::CaseFour::Cost::Penalty::poorTargetingPriority = 1;
unsigned int Strategy::AI::CaseFour::Cost::Penalty::notEngagingEnemy = 5;
unsigned int Strategy::AI::CaseFour::Cost::Penalty::enemyLeftAlive = 5;

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
      heuristic,
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
  ImGui::Text("Penalty customisation:");
  ImGui::Text("Remember, most of these are applied at the end of a turn.");
  ImGui::InputInt("Select unit", (int*)&Cost::Penalty::characterChoice, 0, 30);
  ImGui::InputInt("Unused MP", (int*)&Cost::Penalty::unusedMP, 0, 30);
  ImGui::InputInt("Unused AP", (int*)&Cost::Penalty::unusedAP, 0, 30);
  ImGui::InputInt("Friendly fire", (int*)&Cost::Penalty::friendlyFire, 0, 30);
  ImGui::InputInt("Shot missed", (int*)&Cost::Penalty::missShot, 0, 30);
  ImGui::InputInt("Ally exposed", (int*)&Cost::Penalty::exposedToEnemy, 0, 30);
  ImGui::InputInt("Unnecessary risk", 
      (int*)&Cost::Penalty::unnecessaryRisk, 0, 30);
  ImGui::InputInt("Poor targeting priority", 
      (int*)&Cost::Penalty::poorTargetingPriority, 0, 30);
  ImGui::InputInt("Not engaging enemy", 
      (int*)&Cost::Penalty::notEngagingEnemy, 0, 30);
  ImGui::InputInt("Enemy left alive", 
      (int*)&Cost::Penalty::enemyLeftAlive, 0, 30);
  ImGui::PopItemWidth();
}


///////////////////////////////////////////
// AI COMPONENTS
///////////////////////////////////////////

// Check to see if a State is an endpoint for decision making
bool 
Strategy::AI::CaseFour::isStateEndpoint(const GameState& a, const GameState& b) {
  return Game::hasTurnEnded(a, b);
}


// Estimate the Cost of completing a turn from the current State 
Strategy::AI::CaseFour::Cost 
Strategy::AI::CaseFour::heuristic(const GameState& state) {
  return minimumCost;
}

// Evaluate how good an action is going to be
Strategy::AI::CaseFour::Cost 
Strategy::AI::CaseFour::weighAction(
    const GameState& start,
    const GameState& from, 
    const GameState& to,
    const Action& action) {

  // Prepare to calculate a cost
  Cost cost = minimumCost;

  // This function will evaluate if the chosen action is cost-free or not,
  // free-cost actions are method we will funnel AStar's path
  // The questions posed should be designed in order of ease to achieve
  // The penalty for failing the question should be relative to the satisfaction
  const auto& team = start.currentTeam;
  
  // There is no penalty in switching characters
  if (action.tag == Action::Tag::SelectUnit || 
      action.tag == Action::Tag::CancelSelection) {
    cost.value = Cost::Penalty::characterChoice;
  }

  // Are we killing an enemy with an attack?
  // - Apply a penalty for hitting nothing OR hitting an ally
  else if (action.tag == Action::Tag::Attack) {
    const auto& object = Game::readMap(from.map, action.location);
    if (isUnit(object.second)) {
      if (object.first == team) {
         cost.value = Cost::Penalty::friendlyFire;
      }
    }

    // Check what non-unit objects are being shot
    else {

      // Are we shooting something worthwhile?
      // - Penalise shooting nothing
      if (object.second == Object::Nothing) {
        cost.value = Cost::Penalty::missShot;
      }

      // If we're shooting a wall
      else if (object.second == Object::Wall) {

        // Are we prioritising the killing of enemies over walls
        // - Penalise shooting walls when there are enemies to shoot
        const auto& previousInSight = 
            Game::getUnitsInSight(from.map, from.selection);
        if (!previousInSight.empty()) {
          cost.value = Cost::Penalty::notEngagingEnemy;
        }
        else {

          // Does the destroyed wall expose enemies?
          // - Penalise the destruction of walls that don't reveal enemies
          const auto& inSight = Game::getUnitsInSight(to.map, to.selection);
          if (inSight.empty()) {
            cost.value = Cost::Penalty::poorTargetingPriority;
          }
        }
      }
    }
  }

  // Multiple questions to do with movement:
  else if (action.tag == Action::Tag::MoveUnit) {

    // If there's only one enemy with LoS:
    // Get information on sightlines
    const auto& unit = Game::readMap(from.map, from.selection);
    const auto& unitRange = getUnitRange(unit.second);

    // Gather data on the selection's situation
    const auto& enemiesInSight = 
        Game::getUnitsInSight(from.map, from.selection);
    std::vector<std::tuple<Coord, unsigned int, Object, Range>> enemies;

    // Check to see if the selection is in range of enemies
    for (unsigned int i = 0; i < enemiesInSight.size(); ++i) {
      const auto& enemyAndDistance = enemiesInSight[i];
      const auto& object = Game::readMap(from.map, enemyAndDistance.first);
      const auto& enemyRange = getUnitRange(object.second);

      // If the enemy is in range of the current unit, remember
      if (enemyAndDistance.second <= unitRange) {
        enemies.push_back(std::make_tuple(
            enemyAndDistance.first,
            enemyAndDistance.second,
            object.second, 
            enemyRange));
      }
    }

    // Get the number of allies and enemies
    unsigned int allyCount = 0;
    unsigned int enemyCount = 0;
    for (const auto& kvp : from.teams) {
      if (kvp.first != team) {
        enemyCount += kvp.second;
      }
      else {
        allyCount = kvp.second;
      }
    }

    // Count enemies that are a threat
    unsigned int previousThreats = 0;
    unsigned int currentThreats = 0;
    const auto& newSights = Game::getUnitsInSight(to.map, to.selection);
    for (const auto& e : enemiesInSight) {
      const auto& enemy = Game::readMap(from.map, e.first);
      if (getUnitRange(enemy.second) >= e.second) {
        previousThreats += 1;
      }
    }
    for (const auto& e : newSights) {
      const auto& enemy = Game::readMap(to.map, e.first);
      if (getUnitRange(enemy.second) >= e.second) {
        currentThreats += 1;
      }
    }

    // If the number of allies is less than 75% of the enemy count, run away
    // otherwise, try to attack
    float percentageOfAllies = 1.f;
    if (enemyCount > 0) {
      percentageOfAllies = (float)allyCount / (float)enemyCount;
    }

    // Go on defense if numbers are low
    const bool defenseMode = percentageOfAllies < 0.75f;

    // Try to move in close to enemies, get in range to attack
    if (!defenseMode) {

      // If there are enemy units in sight initially
      if (!enemiesInSight.empty()) {

        // Get closest enemy in the previous state
        auto previousClosestEnemy = enemiesInSight.front();
        for (const auto& e : enemiesInSight) {
          if (e.second < previousClosestEnemy.second) {
            previousClosestEnemy = e;
          }
        }

        // Get closest enemy in the current state
        auto currentClosestEnemy = std::make_pair(Coord(-1, -1), UINT_MAX);
        for (const auto& e : newSights) {
          if (e.second < currentClosestEnemy.second) {
            currentClosestEnemy = e;
          }
        }

        // If there WAS an enemy in the range of the current unit
        if (previousClosestEnemy.second <= unitRange) {
          
          // If there's still an enemy in range
          if (!newSights.empty() && currentClosestEnemy.second <= unitRange) {

            // Penalise for wasting MP on getting closer than necessary
            if (currentClosestEnemy.second < previousClosestEnemy.second) {
              cost.value = Cost::Penalty::unnecessaryRisk;
            }
          }

          // Is there STILL an enemy in the range of the current unit?
          // - Penalise if there's no longer an attackable enemy
          else {
            cost.value = Cost::Penalty::notEngagingEnemy;
          }
        }

        // If there wasn't en enemy in range before
        else {

          // Only applies if there are actually enemies in sight now
          if (!newSights.empty()) {
          
            // Are we moving closer to an enemy unit?
            // - Penalise making the distance to the enemy larger
            if (currentClosestEnemy.second >= previousClosestEnemy.second) {
              cost.value = Cost::Penalty::notEngagingEnemy;
            }
          }
        }
      }

      // If there's no enemies in sight initially
      else {

        // If this move means that there's no enemies in sight
        // DO NOT penalise, as it means we can attack
        if (!newSights.empty()) {
          //return minimumCost;
        }

        // If there's still no enemies in sight, penalise moving away
        else {

          // Simply penalise moving away from enemies
          float previousAverageDistanceToEnemies = 0.f;
          for (const auto& e : from.map.field) {
            const auto& pos = Game::indexToCoord(from.map, e.first);
            auto comps = sf::Vector2f(
              abs((int)from.selection.x - (int)pos.x),
              abs((int)from.selection.y - (int)pos.y));
            comps.x *= comps.x;
            comps.y *= comps.y;
            const auto dist = sqrt(comps.x + comps.y);
            previousAverageDistanceToEnemies += dist;
          }

          float currentAverageDistanceToEnemies = 0.f;
          for (const auto& e : to.map.field) {
            const auto& pos = Game::indexToCoord(to.map, e.first);
            auto comps = sf::Vector2f(
              abs((int)to.selection.x - (int)pos.x),
              abs((int)to.selection.y - (int)pos.y));
            comps.x *= comps.x;
            comps.y *= comps.y;
            const auto dist = sqrt(comps.x + comps.y);
            currentAverageDistanceToEnemies += dist;
          }

          // Are we moving closer to the enemy to try and get in LoS?
          // - Penalise making the average distance to the enemy larger
          if (currentAverageDistanceToEnemies >=
              previousAverageDistanceToEnemies) {
            cost.value = Cost::Penalty::notEngagingEnemy;
          }
        }
      }
    }

    // Move away from enemies, get out of range / line of sight
    else {

      // Are we safe from enemies we cannot kill this turn?
      // - Penalise making the number of threats greater from movement
      if (previousThreats == 0) {
        if (currentThreats > 1) {
          cost.value = Cost::Penalty::exposedToEnemy;
        }
      }

      // Are we still in a position to attack after repositioning?
      // - Penalise losing the target or running into more enemies
      else if (previousThreats == 1) {
        if (currentThreats == 0) {
          cost.value = Cost::Penalty::notEngagingEnemy;
        }
        else if(currentThreats >= previousThreats) {
          cost.value = Cost::Penalty::exposedToEnemy;
        }
      }

      // If there's too many enemies, moving won't do much good
      // - Objectively, more than 2 enemies is overkill for securing a kill
      // - The AI may need to run past MORE enemies to escape
      // Thus, don't penalise for this tough position
      else {
        //return minimumCost;
      }
    }
  }

  // Are we ending a turn with the least amount of resources wasted? 
  // - Ending turn applies a penalty equal to remaining AP and MP
  else if (action.tag == Action::Tag::EndTurn) {

    // Count number of enemies
    unsigned int startEnemyCount = 0;
    for (const auto& t : start.teams) {
      if (t.first != team) {
        startEnemyCount += t.second;
      }
    }
    unsigned int enemyCount = 0;
    unsigned int allyCount = 0;
    for (const auto& t : from.teams) {
      if (t.first != team) {
        enemyCount += t.second;
      }
      else {
        allyCount += t.second;
      }
    }

    // Work out how many enemies have been killed
    unsigned int enemiesKilled = enemyCount < startEnemyCount ?
        startEnemyCount - enemyCount : 0;

    // Count how many enemies were in sight
    // - This stops expecting kills when the enemy wasn't in sight
    const auto& counts = Game::getAlliesAndEnemiesInRange(start, team);
    const unsigned int recomendedKillcount = std::min(std::min(
        counts.second, allyCount), (unsigned int) start.remainingAP);

    // Count the difference in kills
    const unsigned int killsMissed = enemiesKilled < recomendedKillcount
        ? recomendedKillcount - enemiesKilled : 0;

    // Accumulate cost based on enemies remaining and MP/AP wasted
    cost.value = from.remainingMP * Cost::Penalty::unusedMP
        + from.remainingAP * Cost::Penalty::unusedAP
        + killsMissed * Cost::Penalty::enemyLeftAlive;
  }

  // Return the calculated cost of this action
  return cost;
}
