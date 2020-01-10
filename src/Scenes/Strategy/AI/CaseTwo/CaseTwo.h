// CaseTwo.h
// A wrapper containing the A* class

#ifndef CASETWO_H
#define CASETWO_H

#include <utility>
#include "../../../../Controller/AStar/AStar.h"
#include "../../Strategy.h"
#include "../BaseCase.h"

// Encapsulate Strategy AIs
namespace Strategy::AI {

  // Functor for using A*
  class CaseTwo : public BaseCase {
    public:

      // Process the decision
      std::pair<bool, std::stack<Action>> 
          operator()(const GameState& state) override;

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

        // Used to make the AI use their resources
        unsigned int unusedMPPenalty = 0;
        unsigned int unusedAPPenalty = 0;

        // Combine Costs by adding their components
        inline Cost operator+ (const Cost& c) const {
          return Cost{
            remainingEnemyPenalty + c.remainingEnemyPenalty,
            lostAlliesPenalty + c.lostAlliesPenalty,
            alliesAtRiskPenalty + c.alliesAtRiskPenalty,
            unusedMPPenalty + c.unusedMPPenalty,
            unusedAPPenalty + c.unusedAPPenalty,
          };
        }

        // Allow a cost to be scaled
        inline Cost operator* (unsigned int m) const {
          return Cost{
            remainingEnemyPenalty * m,
            lostAlliesPenalty * m,
            alliesAtRiskPenalty * m,
            unusedMPPenalty * m,
            unusedAPPenalty * m
          };
        }
      };

    // Use a personality to compare Cost components
    struct Personality {

      // Modification multipliers for prioritising aspects of the game
      float remainingEnemyMultiplier = 1.f;
      float lostAlliesMultiplier = 1.f;
      float alliesAtRiskMultiplier = 1.f;
      float unusedMPMultiplier = 5.f;
      float unusedAPMultiplier = 5.f;

      // Ask the personality to compare two costs
      constexpr bool operator()(const Cost& a, const Cost& b) const {

        // Use personality's preferences to modify 'true' values
        // This allows the AI to ignore or prioritise things
        const float costA = 
            a.remainingEnemyPenalty * remainingEnemyMultiplier +
            a.lostAlliesPenalty * lostAlliesMultiplier +
            a.alliesAtRiskPenalty * alliesAtRiskMultiplier +
            a.unusedMPPenalty * unusedMPMultiplier +
            a.unusedAPPenalty * unusedAPMultiplier;
        const float costB = 
            b.remainingEnemyPenalty * remainingEnemyMultiplier +
            b.lostAlliesPenalty * lostAlliesMultiplier +
            b.alliesAtRiskPenalty * alliesAtRiskMultiplier +
            b.unusedMPPenalty * unusedMPMultiplier +
            b.unusedAPPenalty * unusedAPMultiplier;

        // Compare ultimate values
        return costA < costB;
      }
    };

    private:

      // Store an A* functor
      Controller::AStar<GameState, Action, Cost> astar;

      // AI's personality
      Personality personality;

      // Overall cost multiplier when ending the turn
      static unsigned int endTurnMultiplier;

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
      static constexpr Cost minimumCost = Cost{0, 0, 0, 0, 0};
      static constexpr Cost maximumCost = Cost{UINT_MAX, UINT_MAX, UINT_MAX,
          UINT_MAX, UINT_MAX};
  };
}

#endif
