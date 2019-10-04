// Strategy/GameState.h
// A struct containing the state of the game's play field

#ifndef STRATEGY_GAMESTATE_H
#define STRATEGY_GAMESTATE_H

#include <SFML/Graphics.hpp>
#include <map>
#include <utility>

#include "Objects.h"
#include "Common.h"

// Seperate Strategy related classes from other games
namespace Strategy {

  // Store positions of important pieces
  struct GameState {

    // Size of the current battlefield
    sf::Vector2u size = sf::Vector2u(8, 8);

    // Map of notable pieces of obstacles
    std::map<sf::Vector2u, std::pair<Team, Object>> field;

  };
}

#endif
