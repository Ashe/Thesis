// Strategy/GameState.h
// A struct containing the state of the game

#ifndef STRATEGY_GAMESTATE_H
#define STRATEGY_GAMESTATE_H

#include "Objects.h"
#include "Common.h"
#include "Map.h"

// Seperate Strategy related classes from other games
namespace Strategy {

  // Store positions of important pieces
  struct GameState {

    // The map of the game
    Map map;

  };
}

#endif
