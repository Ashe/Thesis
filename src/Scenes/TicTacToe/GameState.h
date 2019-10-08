// TicTacToe/GameState.h
// Struct used to represent game state in the Tic-Tac-Toe game

#ifndef TICTACTOE_GAMESTATE_H
#define TICTACTOE_GAMESTATE_H

#include "../../Console.h"
#include "Common.h"

// Encapsulate TicTacToe related classes
namespace TicTacToe {

  // Simple gamestate structure
  struct GameState {

    // Starting player
    static const Player firstPlayer;

    // Turn number (indexed at 1 for ease of reading)
    unsigned int turnNumber = 1;

    // Track who's turn it is
    Player currentTurn = firstPlayer;

    // State of the board
    Player boardState[BOARDSIZE][BOARDSIZE] = {
      { Player::N, Player::N, Player::N },
      { Player::N, Player::N, Player::N },
      { Player::N, Player::N, Player::N }
    };

    // Debug print function to spit out the board state to terminal
    void printBoardState() const {
      for (int j = 0; j < BOARDSIZE; ++j) {
        std::string str = "";
        for (int i = 0; i < BOARDSIZE; ++i) {
          if (boardState[j][i] == Player::N) { str += "N "; }
          else if (boardState[j][i] == Player::X) { str += "X "; }
          else if (boardState[j][i] == Player::O) { str += "O "; }
        }
        Console::log(str.c_str());
      }
    }
  };

  // Allow comparison of GameStates
  // This just checks whether the boardstates are the same
  inline bool operator== (const GameState& a, const GameState& b) {
    for (int j = 0; j < BOARDSIZE; ++j) {
      for (int i = 0; i < BOARDSIZE; ++i) {
        if (a.boardState[j][i] != b.boardState[j][i]) {
          return false;
        }
      }
    }
    return true;
  }
}

// Hash function
// Taken from:
// https://stackoverflow.com/questions/20511347/a-good-hash-function-for-a-vector
namespace std {
  template <> struct hash<TicTacToe::GameState> {
    size_t operator() (const TicTacToe::GameState& st) const {
      std::size_t seed = BOARDSIZE * BOARDSIZE;
      for (int j = 0; j < BOARDSIZE; ++j) {
        for (int i = 0; i < BOARDSIZE; ++i) {
          seed ^= static_cast<unsigned int>(st.boardState[j][i]) + 0x9e3779b9 +
              (seed << 6) + (seed >> 2);
        }
      }
      return std::size_t();
    }
  };
}

#endif
