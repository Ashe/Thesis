// TicTacToe/Cost.h
// Struct to represent the value of moves made in Tic-Tac-Toe

#ifndef TICTACTOE_COST_H
#define TICTACTOE_COST_H

#include <climits>

// Encapsulate TicTacToe related classes
namespace TicTacToe {

  // Store values to help evaluate the cost of making a move
  // Remember that its a COST system so that larger costs can be ruled out
  struct Cost {
    unsigned int logicPenalty = 0;
  };

  // Compare Costs
  inline bool operator< (const Cost& a, const Cost& b) {
    return a.logicPenalty < b.logicPenalty;
  }

  // Combine Costs
  inline Cost operator+ (const Cost& a, const Cost& b) {
    return Cost{ a.logicPenalty + b.logicPenalty};
  }

  // These are BAD penalties
  constexpr unsigned int opponentNearWin = 10;
  constexpr unsigned int opponentNearWinAdditional = 20;
  constexpr unsigned int unnocupiedPenalty = 1;

  // Here's some reductions
  constexpr unsigned int nearWinInitialBonus = 2;
  constexpr unsigned int nearWinAdditionalBonus = 5;

  // Important values
  constexpr Cost minimumCost = Cost{0};
  constexpr Cost maximumCost = Cost{UINT_MAX};
}

#endif
