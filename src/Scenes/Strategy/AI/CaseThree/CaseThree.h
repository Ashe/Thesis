// CaseThree.h
// A wrapper containing the A* class

#ifndef CASETHREE_H
#define CASETHREE_H

#include <utility>
#include "../../../../Controller/AStar/AStar.h"
#include "../../Strategy.h"
#include "../BaseCase.h"

// Encapsulate Strategy AIs
namespace Strategy::AI {

  // Functor for using A*
  class CaseThree : public BaseCase {
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
          unsigned int characterChoice = 1;
          unsigned int unusedMP = 5;
          unsigned int unusedAP = 10;
          unsigned int friendlyFire = 25;
          unsigned int missShot = 25;

          // Playstyle penalties: Defensive
          unsigned int exposedToEnemy = 5;
          unsigned int unnecessaryRisk = 5;

          // Playstyle penalties: Offensive
          unsigned int poorTargetingPriority = 1;
          unsigned int notEngagingEnemy = 5;
          unsigned int enemyLeftAlive = 5;
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

      // Store values for giving penalties
      Cost::Penalty penalties;

      // Determine what a goal is
      bool isStateEndpoint(const GameState& a, const GameState& b);

      // Estimate the Cost of completing a turn from the current State 
      Cost heuristic(const GameState& state);

      // Evaluate how good an action is going to be
      Cost weighAction(
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
