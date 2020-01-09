// CaseOne.h
// A wrapper containing the A* class

#ifndef CASEONE_H
#define CASEONE_H

#include <utility>
#include "../../../../Controller/AStar/AStar.h"
#include "../../Strategy.h"
#include "../Common.h"


// Encapsulate Strategy AIs
namespace Strategy::AI {

  // Functor for using A*
  class CaseOne : public BaseCase {
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
          static constexpr unsigned int characterChoice = 1;
          static constexpr unsigned int unusedMP = 5;
          static constexpr unsigned int unusedAP = 10;
          static constexpr unsigned int friendlyFire = 25;
          static constexpr unsigned int missShot = 25;

          // Playstyle penalties: Defensive
          static constexpr unsigned int exposedToEnemy = 5;
          static constexpr unsigned int unnecessaryRisk = 5;

          // Playstyle penalties: Offensive
          static constexpr unsigned int poorTargetingPriority = 1;
          static constexpr unsigned int notEngagingEnemy = 5;
          static constexpr unsigned int enemyLeftAlive = 5;
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
      Controller::AStar<GameState, Action, CaseOne::Cost> astar;

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
