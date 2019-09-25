// Controller/Random.h
// A controller for randomly making moves without making decisions

#ifndef CONTROLLER_RANDOM_H
#define CONTROLLER_RANDOM_H

#include <vector>
#include <utility>
#include <algorithm>
#include <functional>
#include <random>

// Seperate functions here from other controllers
namespace Controller::Random {

  // Evaluates options and returns a new state after taking action
  // Templates: State, Action
  template <class S, class A>
  std::pair<bool, S> decide(
      const S& state,
      std::function<std::vector<A>(const S&)> getOptions,
      std::function<std::pair<bool, const S>(const S&, const A&)> takeAction) {

    // Get options to play
    std::vector<A> options = getOptions(state);

    // Make a random-number-generator
    std::random_device random;
    std::mt19937 generator(random());

    // Shuffle available options
    std::shuffle(options.begin(), options.end(), generator);

    // Try to make moves, and if one succeeds, return it
    for (auto i = options.begin(); i != options.end(); ++i) {
      const auto attempt = takeAction(state, *i);
      if (attempt.first) {
        return attempt;
      }
    }

    // Return unsuccessfully with the current state
    return std::make_pair(false, state);
  }
}

#endif
