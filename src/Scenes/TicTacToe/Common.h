// TicTacToe/Common.h
// Definition of common types and functions used in Tic-Tac-Toe

#ifndef TICTACTOE_COMMON_H
#define TICTACTOE_COMMON_H

// Keep dependencies to a minimum
#include <SFML/Graphics.hpp>

// Encapsulate Tic-Tac-Toe only types
namespace TicTacToe {

  // Representation of moves
  typedef sf::Vector2i Move;

  // Representation of players
  enum class Player {
    N,
    X,
    O
  };

  // Convert player enum to string
  inline std::string playerToString(const Player& player) {
    switch(player) {
      case Player::X: return "X"; break;
      case Player::O: return "O"; break;
      default: return ""; break;
    }
  }
}

#endif
