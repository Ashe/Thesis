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
    Coord location = Coord(-1, -1);
  };

  // Converts an action into a string
  constexpr const char* actionToString(const Action& action) {
    switch (action.tag) {
      case Action::Tag::SelectUnit: return "Select unit"; break;
      case Action::Tag::CancelSelection: return "Deselect unit"; break;
      case Action::Tag::MoveUnit: return "Move unit"; break;
      case Action::Tag::Attack: return "Attack"; break;
      case Action::Tag::EndTurn: return "End turn"; break;
      default: return "Unknown Action";
    }
  }
}

#endif
