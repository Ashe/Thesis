// Strategy/GameState.h
// A struct containing the state of the game

#ifndef STRATEGY_GAMESTATE_H
#define STRATEGY_GAMESTATE_H

#include <string>
#include <map>

#include "../../Console.h"
#include "Objects.h"
#include "Common.h"
#include "Map.h"

// Seperate Strategy related classes from other games
namespace Strategy {

  // Store positions of important pieces
  struct GameState {

    // The map of the game
    Map map;

    // Member count of participating teams
    std::map<Team, unsigned int> teams;
    Team currentTeam;

    // Track the currently selected piece
    Coord selection = Coord(-1, -1);
  };

  // Allow comparison of GameStates
  // This just checks whether the boardstates are the same
  inline bool operator== (const GameState& a, const GameState& b) {
    return a.currentTeam == b.currentTeam
      && a.selection == b.selection
      && a.teams == b.teams
      && a.map.size == b.map.size
      && a.map.field == b.map.field;
  }
}

// Hash function
// @NOTE: Very inefficient but it'll force the map to use the == operator
// For our project this is all we need
namespace std {
  template <> struct hash<Strategy::GameState> {
    size_t operator() (const Strategy::GameState& st) const {
      return std::size_t();
    }
  };
}

#endif
