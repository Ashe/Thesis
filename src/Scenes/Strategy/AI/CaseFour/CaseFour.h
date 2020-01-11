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
          static unsigned int optionalActionPenalty;
          static unsigned int selectUnit;
          static unsigned int spentMP;
          static unsigned int spentAP;
          static unsigned int turnEnded;
          static unsigned int attackedNothing;
          static unsigned int attackedFriendly;
        };

        struct Predictions {
          static unsigned int allyNeedsSaving;
          static unsigned int alliesFurtherExposed;
          static unsigned int enemyNeedsEliminating;
          static unsigned int enemyNeedsExposing;
          static unsigned int needToMoveCloser;
        };

        // The actual value of the penalty
        unsigned int value = 0;

        // Operators
        Cost operator+(const Cost& c) const { return Cost { value + c.value }; }
        Cost operator*(unsigned int m) const { return Cost { value * m }; }
        bool operator<(const Cost& c) const { return value < c.value; }
        bool operator==(const Cost& c) const { return value == c.value; }

      };

      // Heuristic functor that takes the starting state
      // Used for checking if conditions are worsening etc.
      struct HeuristicFunctor {
        GameState startingState;
        HeuristicFunctor(const GameState& state, bool& goal);
        unsigned int allyCount = 0;
        unsigned int enemyCount = 0;
        unsigned int alliesInRange = 0;
        unsigned int enemiesInRange = 0;
        bool& useGoal;
        Cost operator()(const GameState& state);
      };

    private:

      // Store an A* functor
      Controller::AStar<GameState, Action, Cost> astar;

      // Determine what a goal is
      static bool isStateEndpoint(const GameState& a, const GameState& b);

      // Estimate the Cost of completing a turn from the current State 
      static Cost heuristic(const GameState& state);

      // Should the AI be forced to get closer?
      static bool enableGoalMoveOrKill;

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
