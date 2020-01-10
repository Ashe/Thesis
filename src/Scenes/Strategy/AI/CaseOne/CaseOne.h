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

        // Used to prioritise winning the game
        unsigned int remainingEnemyPenalty = 0;

        // Used to prioritise staying alive
        unsigned int lostAlliesPenalty = 0;

        // Used to increase chances of survival by staying hidden
        unsigned int alliesAtRiskPenalty = 0;

        // Combine Costs by adding their components
        inline Cost operator+ (const Cost& c) const {
          return Cost{
            remainingEnemyPenalty + c.remainingEnemyPenalty,
            lostAlliesPenalty + c.lostAlliesPenalty,
            alliesAtRiskPenalty + c.alliesAtRiskPenalty,
          };
        }

        // Allow a cost to be scaled
        inline Cost operator* (unsigned int m) const {
          return Cost{
            remainingEnemyPenalty * m,
            lostAlliesPenalty * m,
            alliesAtRiskPenalty * m
          };
        }
      };

    // Use a personality to compare Cost components
    struct Personality {

      // Modification multipliers for prioritising aspects of the game
      float remainingEnemyMultiplier = 1.f;
      float lostAlliesMultiplier = 1.f;
      float alliesAtRiskMultiplier = 1.f;
      float unusedMPMultiplier = 1.f;
      float unusedAPMultiplier = 1.f;

      // Ask the personality to compare two costs
      constexpr bool operator()(const Cost& a, const Cost& b) const {

        // Use personality's preferences to modify 'true' values
        // This allows the AI to ignore or prioritise things
        const float costA = 
            a.remainingEnemyPenalty * remainingEnemyMultiplier +
            a.lostAlliesPenalty * lostAlliesMultiplier +
            a.alliesAtRiskPenalty * alliesAtRiskMultiplier;
        const float costB = 
            b.remainingEnemyPenalty * remainingEnemyMultiplier +
            b.lostAlliesPenalty * lostAlliesMultiplier +
            b.alliesAtRiskPenalty * alliesAtRiskMultiplier;

        // Compare ultimate values
        return costA < costB;
      }
    };

    private:

      // Store an A* functor
      Controller::AStar<GameState, Action, CaseOne::Cost> astar;

      // AI's personality
      Personality personality;

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
      static constexpr Cost minimumCost = Cost{0, 0, 0};
      static constexpr Cost maximumCost = Cost{UINT_MAX, UINT_MAX, UINT_MAX};
  };
}

#endif
