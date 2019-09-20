// Random.h
// A controller for randomly making moves without making decisions

#ifndef RANDOM_H
#define RANDOM_H

#include <vector>
#include <utility>
#include <algorithm>
#include <functional>
#include <random>

// Seperate functions here from other controllers
namespace RandomController {

  // Evaluates options and returns a new state after making a move
  template <class S, class P, class M>
  std::pair<bool, S> takeTurn(
      const S& state,
      std::function<std::vector<M>(const S&)> getOptions,
      std::function<std::pair<bool, const S>(const S&, const M&)> makeMove) {

    // Get options to play
    std::vector<M> options = getOptions(state);

    // Make a random-number-generator
    std::random_device random;
    std::mt19937 generator(random());

    // Shuffle available options
    std::shuffle(options.begin(), options.end(), generator);

    // Try to make moves, and if one succeeds, return it
    for (auto i = options.begin(); i != options.end(); ++i) {
      const auto attempt = makeMove(state, *i);
      if (attempt.first) {
        return attempt;
      }
    }

    // Return unsuccessfully with the current state
    return std::make_pair(false, state);
  }
}

#endif
