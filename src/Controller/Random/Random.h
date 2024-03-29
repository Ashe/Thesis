// Controller/Random.h
// A controller for randomly making moves without making decisions

#ifndef CONTROLLER_RANDOM_H
#define CONTROLLER_RANDOM_H

#include <vector>
#include <stack>
#include <utility>
#include <algorithm>
#include <functional>
#include <random>

// Seperate functions here from other controllers
namespace Controller::Random {

  // Evaluates options and returns a vector of actions
  // Templates: State, Action
  template <class S, class A>
  std::pair<bool, std::stack<A>> decide(
      const S& state,
      std::function<std::vector<A>(const S&)> getOptions,
      std::function<bool(const S&, const S&)> isStateEndpoint,
      std::function<std::pair<bool, S>(const S&, const A&)> takeAction) {

    // Prepare to generate a random list of actions
    std::vector<A> actions;
    std::random_device random;
    std::mt19937 generator(random());
    S currentState = state;

    // Continue making decisions until endpoint reached
    while (!isStateEndpoint(state, currentState)) {

      // Get options to play
      std::vector<A> options = getOptions(currentState);

      // Shuffle available options
      std::shuffle(options.begin(), options.end(), generator);

      // Try to make moves, and if one succeeds, return it
      bool success = false;
      for (auto i = options.cbegin(); !success && i != options.cend(); ++i) {
        const A action = *i;
        const auto attempt = takeAction(currentState, action);
        if (attempt.first) {
          actions.push_back(action);
          currentState = attempt.second;
          success = true;
        }
      }

      // If no moves could be made with no end point reached, return fail
      if (!success) {
        return std::make_pair(false, std::stack<A>());
      }
    }

    // Return the actions to take
    std::stack<A> actionsToTake;
    for (auto it = actions.crbegin(); it != actions.crend(); ++it) {
      actionsToTake.push(*it);
    }
    return std::make_pair(true, actionsToTake);
  }
}

#endif
