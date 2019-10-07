// Strategy/GameState.h
// A struct containing the state of the game

#ifndef STRATEGY_GAMESTATE_H
#define STRATEGY_GAMESTATE_H

#include <map>

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
}

#endif
