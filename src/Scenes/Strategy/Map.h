// Strategy/Map.h
// A struct containing the game's map

#ifndef STRATEGY_MAP_H
#define STRATEGY_MAP_H

#include <SFML/Graphics.hpp>
#include <map>

#include "Objects.h"
#include "Common.h"

// Seperate Strategy related classes from other games
namespace Strategy {

  // Store all data related to the game's map
  struct Map {

    // Size of the current battlefield
    Coord size = Coord(5, 5);

    // Map of notable pieces of obstacles
    std::map<unsigned int, std::pair<Team, Object>> field;

  };
}
#endif
