// Strategy/Map.h
// A struct containing the game's map

#ifndef STRATEGY_MAP_H
#define STRATEGY_MAP_H

#include <map>
#include <istream>
#include <ostream>
#include <string>

#include "../../Console.h"
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

    // Starting movement point amount for this map
    Points startingMP = 5;

    // Starting action point amount for this map
    Points startingAP = 3;
  };

  // Save the map to a stream
  std::ostream& operator<< (std::ostream& os, const Map& m);

  // Load a map from a stream
  std::istream& operator>> (std::istream& is, Map& m);
}
#endif
