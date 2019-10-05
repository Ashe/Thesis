// Strategy/Common.h
// Common functions and variables for use in the game

#ifndef STRATEGY_COMMON_H
#define STRATEGY_COMMON_H

#include <SFML/Graphics.hpp>

// Seperate strategy classes from other games
namespace Strategy {

  // A Strategy::Team is just an int
  typedef unsigned int Team;

  // Coordinate in the game
  typedef sf::Vector2u Coord;

};

#endif
