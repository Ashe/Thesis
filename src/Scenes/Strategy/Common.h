// Strategy/Common.h
// Common functions and variables for use in the game

#ifndef STRATEGY_COMMON_H
#define STRATEGY_COMMON_H

#include <map>
#include <SFML/Graphics.hpp>

// Seperate strategy classes from other games
namespace Strategy {

  // A Strategy::Team is just an int
  typedef unsigned int Team;

  // Coordinate in the game
  typedef sf::Vector2i Coord;

  // Team colours
  static std::map<Team, sf::Color> teamColours = {
    {0, sf::Color::Cyan},
    {1, sf::Color::Red},
    {2, sf::Color::Green},
    {3, sf::Color::Yellow}
  };

  // Different methods of drawing an object
  enum class RenderStyle {
    NotPlaying,
    Playing,
    Hovered,
    Selected,
    Ghost
  };
};

#endif
