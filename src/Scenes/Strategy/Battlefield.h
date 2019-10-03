// Strategy/Battlefield.h
// A struct containing the state of the game's play field

#ifndef STRATEGY_BATTLEFIELD_H
#define STRATEGY_BATTLEFIELD_H

#include <SFML/Graphics.hpp>

// Seperate strategy related classes from each other
namespace Strategy {

  // Store positions of important pieces
  struct Battlefield {

    // Size of the battlefield
    sf::Vector2u size = sf::Vector2u(25, 25);

  };
}

#endif
