// Strategy/Cost.h
// A struct used to evaluate Actions in the strategy game

#ifndef STRATEGY_COST_H
#define STRATEGY_COST_H

#include <algorithm>
#include <climits>

// Seperate Strategy related classes from other games
namespace Strategy {

  // Store values to help evaluate the cost of taking an Action
  // Remember that its a COST system so that larger costs can be ruled out
  struct Cost {

    // Count enemies that are still alive
    // - Used to prioritise winning the game
    unsigned int remainingEnemyPenalty = 0;

    // Count allies that have died this turn
    // - Used to prioritise staying alive
    unsigned int lostAlliesPenalty = 0;

    // Count allies that are in range of the enemy
    // - Used to increase chances of survival by staying hidden
    unsigned int alliesAtRiskPenalty = 0;

    // Count the enemies out of range
    // - Used to incentivise getting close (but out ranging) the enemy
    unsigned int enemiesOutOfRangePenalty = 0;
  };

  // When combining costs, take the latter
  // - All that matters is the current state
  inline Cost operator+ (const Cost& a, const Cost& b) {
   return Cost{
     a.remainingEnemyPenalty + b.remainingEnemyPenalty,
     a.lostAlliesPenalty + b.lostAlliesPenalty,
     a.alliesAtRiskPenalty + b.alliesAtRiskPenalty,
     a.enemiesOutOfRangePenalty + b.enemiesOutOfRangePenalty
   };
  }

  // Allow a cost to be scaled
  inline Cost operator* (const Cost& c, unsigned int m) {
    return Cost{
      c.remainingEnemyPenalty * m,
      c.lostAlliesPenalty * m,
      c.alliesAtRiskPenalty * m,
      c.enemiesOutOfRangePenalty * m,
    };
  }

  // Use a personality to compare Cost components
  struct Personality {

    // Modification multipliers for prioritising aspects of the game
    float remainingEnemyMultiplier = 1.f;
    float lostAlliesMultiplier = 1.f;
    float alliesAtRiskMultiplier = 1.f;
    float enemiesOutOfRangeMultiplier = 1.f;

    // Ask the personality to compare two costs
    constexpr bool operator()(const Cost& a, const Cost& b) const {

      // Use personality's preferences to modify 'true' values
      // This allows the AI to ignore or prioritise things
      const float costA = 
          a.remainingEnemyPenalty * remainingEnemyMultiplier +
          a.lostAlliesPenalty * lostAlliesMultiplier +
          a.alliesAtRiskPenalty * alliesAtRiskMultiplier +
          a.enemiesOutOfRangePenalty * enemiesOutOfRangeMultiplier;
      const float costB = 
          b.remainingEnemyPenalty * remainingEnemyMultiplier +
          b.lostAlliesPenalty * lostAlliesMultiplier +
          b.alliesAtRiskPenalty * alliesAtRiskMultiplier +
          b.enemiesOutOfRangePenalty * enemiesOutOfRangeMultiplier;

      // Compare ultimate values
      return costA < costB;
    }
  };

  // Important values
  constexpr Cost minimumCost = Cost{ 0, 0, 0 };
  constexpr Cost maximumCost = Cost{ UINT_MAX, UINT_MAX, UINT_MAX };
}

#endif
