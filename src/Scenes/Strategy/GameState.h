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

    // Current turn number
    unsigned int turnNumber = 0;

    // The map of the game
    Map map;

    // Member count of participating teams
    std::map<Team, unsigned int> teams;
    Team currentTeam;

    // Track the currently selected piece
    Coord selection = Coord(-1, -1);

    // Movement points remaining for this turn
    Points remainingMP = 0;

    // Action points remaining for this turn
    Points remainingAP = 0;
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
// It's not a perfect hash, but it will seperate distinct states based on vars
// The inspiration for the hash was found on the following link, 7/11/2019:
// https://codereview.stackexchange.com/questions/171999/specializing-stdhash-for-stdarray
// For our project this is all we need
namespace std {
  template <> struct hash<Strategy::GameState> {
    size_t operator() (const Strategy::GameState& st) const {
      std::hash<int> hasher;
      std::size_t result = 0;
      unsigned int unitsLeft = 0;
      for (const auto& team : st.teams) {
        unitsLeft += team.second;
      }
      const std::vector<int> toHash = {
          (int) st.turnNumber, 
          (int) st.currentTeam, 
          (int) unitsLeft,
          st.selection.x, 
          st.selection.y,
          st.remainingMP, 
          st.remainingAP };
      for (const auto& n : toHash) {
        result = (result << 1) ^ hasher(n);
      }
      return result;
    }
  };
}

#endif
