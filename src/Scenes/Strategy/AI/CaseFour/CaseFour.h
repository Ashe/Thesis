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
          unsigned int optionalActionPenalty = 1;
          unsigned int selectUnit = 3;
          unsigned int spentMP = 0;
          unsigned int spentAP = 0;
          unsigned int turnEnded = 2;
          unsigned int attackedNothing = 20;
          unsigned int attackedWall = 10;
          unsigned int attackedFriendly = 50;
        };

        struct Predictions {
          unsigned int allyNeedsSaving = 2;
          unsigned int alliesFurtherExposed = 4;
          unsigned int enemyNeedsEliminating = 15;
          unsigned int enemyNeedsExposing = 2;
          unsigned int needToMoveCloser = 10;
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

      // Should the AI be forced to get closer?
      bool enableGoalMoveOrKill = true;

      // Variables of starting node
      GameState startingState;
      float startingDistanceToClosestEnemy = 0.f;
      unsigned int startingAllyCount = 0;
      unsigned int startingEnemyCount = 0;
      unsigned int startingAlliesInRange = 0;
      unsigned int startingEnemiesInRange = 0;

      // Store values for evaluating actions
      Cost::Penalty penalties;
      Cost::Predictions predictions;

      // Get actions only if the turn hasn't ended
      std::vector<Action> getActions(const GameState& state);

      // Determine what a goal is
      bool isStateEndpoint(const GameState& a, const GameState& b);

      // How far away is the current node from a goal
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
