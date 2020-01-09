// Strategy/AI/Common.h
// Common code for all case studies to share

#ifndef STRATEGY_AI_COMMON_H
#define STRATEGY_AI_COMMON_H

// Encapsulate all strategy AIs
namespace Strategy::AI {

  // All AI cases need debugging functionality
  class BaseCase {
    public:

      // Cases must allow for grabbing state
      virtual const unsigned int getStatesProcessed() const = 0;

      // Cases must allow for getting the amount of open states
      virtual const unsigned int getOpenStatesRemaining() const = 0;

      // Optional function for adding additional debugging
      virtual void debug() {}
  };
}

#endif
