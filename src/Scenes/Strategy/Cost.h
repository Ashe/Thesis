// Strategy/Cost.h
// A struct used to evaluate Actions in the strategy game

#ifndef STRATEGY_COST_H
#define STRATEGY_COST_H

#include <climits>

// Seperate Strategy related classes from other games
namespace Strategy {

  // Store values to help evaluate the cost of taking an Action
  // Remember that its a COST system so that larger costs can be ruled out
  struct Cost {

    // Static struct of penalties to apply
    struct Penalty {

      // Logic penalties
      static constexpr unsigned int characterChoice = 1;
      static constexpr unsigned int unusedMP = 5;
      static constexpr unsigned int unusedAP = 10;
      static constexpr unsigned int friendlyFire = 25;
      static constexpr unsigned int missShot = 25;

      // Playstyle penalties: Defensive
      static constexpr unsigned int exposedToEnemy = 5;
      static constexpr unsigned int unnecessaryRisk = 5;

      // Playstyle penalties: Offensive
      static constexpr unsigned int notEngagingEnemy = 5;
      static constexpr unsigned int enemyLeftAlive = 5;
    };

    // The actual value of the penalty
    unsigned int value = 0;

  };

  // Important Cost instances
  constexpr Cost minimumCost = Cost{0};
  constexpr Cost maximumCost = Cost{UINT_MAX};

  // Combine costs
  constexpr Cost operator+ (const Cost& a, const Cost& b) {
   return Cost{
     a.value + b.value
   };
  }

  // Allow a cost to be scaled
  constexpr Cost operator* (const Cost& c, unsigned int m) {
    return Cost{
      c.value * m
    };
  }

  // Compare two cost instances
  constexpr bool operator< (const Cost& a, const Cost& b) {
    return a.value < b.value;
  }
  
  // Check two cost instances for equality
  constexpr bool operator== (const Cost& a, const Cost& b) {
    return a.value == b.value;
  }
}

#endif
