// CaseThree.cpp
// A wrapper containing the A* class

#include "CaseThree.h"

///////////////////////////////////////////
// AI CONTROLLER FUNCTIONS
///////////////////////////////////////////

// Start making the decision
std::pair<bool, std::stack<Strategy::Action>> 
Strategy::AI::CaseThree::operator()(const GameState& state) {
  return astar(
      state, 
      minimumCost, 
      maximumCost, 
      Game::getAllPossibleActions,
      std::bind(&CaseThree::isStateEndpoint, this, 
          std::placeholders::_1,
          std::placeholders::_2),
      std::bind(&CaseThree::heuristic, this,
        std::placeholders::_1),
      std::bind(&CaseThree::weighAction, this,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4),
      Game::takeAction,
      std::less<Cost>());
}

// Get number of states processed so far
const unsigned int
Strategy::AI::CaseThree::getStatesProcessed() const {
  return astar.getStatesProcessed();
}

// Get number of open states remaining
const unsigned int
Strategy::AI::CaseThree::getOpenStatesRemaining() const {
  return astar.getRemaining().size();
}

// Debugging functionality
void
Strategy::AI::CaseThree::debug() {
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
  ImGui::InputInt("Select unit", (int*)&penalties.characterChoice, 0, 30);
  ImGui::InputInt("Unused MP", (int*)&penalties.unusedMP, 0, 30);
  ImGui::InputInt("Unused AP", (int*)&penalties.unusedAP, 0, 30);
  ImGui::InputInt("Friendly fire", (int*)&penalties.friendlyFire, 0, 30);
  ImGui::InputInt("Shot missed", (int*)&penalties.missShot, 0, 30);
  ImGui::InputInt("Ally exposed", (int*)&penalties.exposedToEnemy, 0, 30);
  ImGui::InputInt("Unnecessary risk", 
      (int*)&penalties.unnecessaryRisk, 0, 30);
  ImGui::InputInt("Poor targeting priority", 
      (int*)&penalties.poorTargetingPriority, 0, 30);
  ImGui::InputInt("Not engaging enemy", 
      (int*)&penalties.notEngagingEnemy, 0, 30);
  ImGui::InputInt("Enemy left alive", 
      (int*)&penalties.enemyLeftAlive, 0, 30);
  ImGui::PopItemWidth();
}


///////////////////////////////////////////
// AI COMPONENTS
///////////////////////////////////////////

// Check to see if a State is an endpoint for decision making
bool 
Strategy::AI::CaseThree::isStateEndpoint(const GameState& a, const GameState& b) {
  return Game::hasTurnEnded(a, b);
}


// Estimate the Cost of completing a turn from the current State 
Strategy::AI::CaseThree::Cost 
Strategy::AI::CaseThree::heuristic(const GameState& state) {
  return minimumCost;
}

// Evaluate how good an action is going to be
Strategy::AI::CaseThree::Cost 
Strategy::AI::CaseThree::weighAction(
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
  
  // There is nothing wrong in switching characters, but there must be
  // some penalty to stop infinite loops
  if (action.tag == Action::Tag::SelectUnit || 
      action.tag == Action::Tag::CancelSelection) {
    cost.value = penalties.characterChoice;
  }

  // Are we killing an enemy with an attack?
  // - Apply a penalty for hitting nothing OR hitting an ally
  else if (action.tag == Action::Tag::Attack) {
    const auto& object = Game::readMap(from.map, action.location);
    if (isUnit(object.second)) {
      if (object.first == team) {
         cost.value = penalties.friendlyFire;
      }
    }

    // Check what non-unit objects are being shot
    else {

      // Are we shooting something worthwhile?
      // - Penalise shooting nothing
      if (object.second == Object::Nothing) {
        cost.value = penalties.missShot;
      }

      // If we're shooting a wall
      else if (object.second == Object::Wall) {

        // Are we prioritising the killing of enemies over walls
        // - Penalise shooting walls when there are enemies to shoot
        const auto& previousInSight = 
            Game::getUnitsInSight(from.map, from.selection);
        if (!previousInSight.empty()) {
          cost.value = penalties.notEngagingEnemy;
        }
        else {

          // Does the destroyed wall expose enemies?
          // - Penalise the destruction of walls that don't reveal enemies
          const auto& inSight = Game::getUnitsInSight(to.map, to.selection);
          if (inSight.empty()) {
            cost.value = penalties.poorTargetingPriority;
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
              cost.value = penalties.unnecessaryRisk;
            }
          }

          // Is there STILL an enemy in the range of the current unit?
          // - Penalise if there's no longer an attackable enemy
          else {
            cost.value = penalties.notEngagingEnemy;
          }
        }

        // If there wasn't en enemy in range before
        else {

          // Only applies if there are actually enemies in sight now
          if (!newSights.empty()) {
          
            // Are we moving closer to an enemy unit?
            // - Penalise making the distance to the enemy larger
            if (currentClosestEnemy.second >= previousClosestEnemy.second) {
              cost.value = penalties.notEngagingEnemy;
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
          const float previousAverageDistanceToEnemies = 
            Game::getDistanceToClosestEnemy(from.map, from.currentTeam);

          const float currentAverageDistanceToEnemies = 
              Game::getDistanceToClosestEnemy(to.map, from.currentTeam);

          // Are we moving closer to the enemy to try and get in LoS?
          // - Penalise making the average distance to the enemy larger
          if (currentAverageDistanceToEnemies >=
              previousAverageDistanceToEnemies) {
            cost.value = penalties.notEngagingEnemy;
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
          cost.value = penalties.exposedToEnemy;
        }
      }

      // Are we still in a position to attack after repositioning?
      // - Penalise losing the target or running into more enemies
      else if (previousThreats == 1) {
        if (currentThreats == 0) {
          cost.value = penalties.notEngagingEnemy;
        }
        else if(currentThreats >= previousThreats) {
          cost.value = penalties.exposedToEnemy;
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
    cost.value = from.remainingMP * penalties.unusedMP
        + from.remainingAP * penalties.unusedAP
        + killsMissed * penalties.enemyLeftAlive;
  }

  // Return the calculated cost of this action
  return cost;
}
