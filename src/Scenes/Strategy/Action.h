// Strategy/Action.h
// A struct representing the things a controller/player can do

#ifndef STRATEGY_ACTION_H
#define STRATEGY_ACTION_H

#include "Common.h"

// Seperate Strategy releated classes from other games
namespace Strategy {

  // Encapsulate the different types of actions
  struct Action {

    // Default constructor
    Action() { 
      tag = Tag::EndTurn;
      location = Coord(-1, -1);
    }

    // Tag enum to determine action this is
    enum Tag {
      EndTurn,
      CancelSelection,
      SelectUnit,
      MoveUnit,
      Attack
    };
    Tag tag = Tag::EndTurn;

    // Data to help fulfill the different possible actions
    union {
      Coord location;
    };
  };
}

#endif
