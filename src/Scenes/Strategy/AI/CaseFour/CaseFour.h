// CaseFour.h
// A wrapper containing the A* class

#ifndef CASEFOUR_H
#define CASEFOUR_H

#include <utility>
#include "../../../../Controller/AStar/AStar.h"
#include "../../Strategy.h"
#include "../BaseCase.h"

// Encapsulate Strategy AIs
namespace Strategy::AI {

  // Functor for using A*
  class CaseFour : public BaseCase {
    public:

      // Process the decision
      std::pair<bool, std::stack<Action>> operator()(const GameState& state);

      // Debugging functions
      const unsigned int getStatesProcessed() const override;
      const unsigned int getOpenStatesRemaining() const override;
      void debug() override;

      // Store values to help evaluate the cost of taking an Action
      // Remember that its a COST system so that larger costs can be ruled out
      struct Cost {

        // Static struct of penalties to apply
        struct Penalty {

          // Logic penalties
          static unsigned int characterChoice;
          static unsigned int unusedMP;
          static unsigned int unusedAP;
          static unsigned int friendlyFire;
          static unsigned int missShot;

          // Playstyle penalties: Defensive
          static unsigned int exposedToEnemy;
          static unsigned int unnecessaryRisk;

          // Playstyle penalties: Offensive
          static unsigned int poorTargetingPriority;
          static unsigned int notEngagingEnemy;
          static unsigned int enemyLeftAlive;
        };

        // The actual value of the penalty
        unsigned int value = 0;

        // Operators
        Cost operator+(const Cost& c) const { return Cost { value + c.value }; }
        Cost operator*(unsigned int m) const { return Cost { value * m }; }
        bool operator<(const Cost& c) const { return value < c.value; }
        bool operator==(const Cost& c) const { return value == c.value; }

      };

    private:

      // Store an A* functor
      Controller::AStar<GameState, Action, Cost> astar;

      // Determine what a goal is
      static bool isStateEndpoint(const GameState& a, const GameState& b);

      // Estimate the Cost of completing a turn from the current State 
      static Cost heuristic(const GameState& state);

      // Evaluate how good an action is going to be
      static Cost weighAction(
          const GameState& start,
          const GameState& from, 
          const GameState& to,
          const Action& action);

      // Important Cost instances
      static constexpr Cost minimumCost = Cost{0};
      static constexpr Cost maximumCost = Cost{UINT_MAX};
  };
}

#endif
