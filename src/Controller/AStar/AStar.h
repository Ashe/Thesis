// Controller/AStar.h
// A controller that employs the A* pathfinding algorithm

#ifndef CONTROLLER_ASTAR_H
#define CONTROLLER_ASTAR_H

#include "../../Console.h"

#include <cassert>
#include <unordered_map>
#include <vector>
#include <stack>
#include <utility>
#include <algorithm>
#include <functional>

// Seperate functions here from other controllers
namespace Controller::AStar {

// // A* finds a path from start to goal.
// // h is the heuristic function. 
// // h(n) estimates the cost to reach goal from node n.
// function A_Star(start, goal, h)
//     // The set of discovered nodes that need to be (re-)expanded.
//     // Initially, only the start node is known.
//     openSet := {start}
//
//     // For node n, history[n] is the node immediately 
//     // preceding it on the cheapest path from start to n currently known.
//     history := an empty map
//
//     // For node n, gScore[n] is the cost of the 
//     // cheapest path from start to n currently known.
//     gScore := map with default value of Infinity
//     gScore[start] := 0
//
//     // For node n, fScore[n] := gScore[n] + h(n).
//     fScore := map with default value of Infinity
//     fScore[start] := h(start)
//
//     while openSet is not empty
//         current := the node in openSet having the lowest fScore[] value
//         if current = goal
//             return reconstruct_path(history, current)
//
//         openSet.Remove(current)
//         closedSet.Add(current)
//         for each neighbor of current
//             if neighbor in closedSet 
//                 continue
//             // d(current,neighbor) is the weight of the edge from current to neighbor
//             // tentative_gScore is the distance from start to the neighbor through current
//             tentative_gScore := gScore[current] + d(current, neighbor)
//             if tentative_gScore < gScore[neighbor]
//                 // This path to neighbor is better than any previous one. Record it!
//                 history[neighbor] := current
//                 gScore[neighbor] := tentative_gScore
//                 fScore[neighbor] := gScore[neighbor] + h(neighbor)
//                 if neighbor not in openSet
//                     openSet.add(neighbor)
//
//       // Open set is empty but goal was never reached
//       return failure

  // Evaluates options and returns a stack of actions to take
  // Templates: thought STATE, ACTION, decision COST
  template <class S, class A, class C>
  std::pair<bool, std::stack<A>> decide(
      const S& startingState,
      const C& minimumCost,
      const C& maximumCost,
      std::function<std::vector<A>(const S&)> getPossibleActions,
      std::function<bool(const S&, const S&)> isStateGoal,
      std::function<C(const S&)> heuristic,
      std::function<C(const S&, const S&, const A&)> weighAction,
      std::function<std::pair<bool, const S>(const S&, const A&)> takeAction,
      std::function<bool(const C&, const C&)> compareCost) {// = std::min) {

    // All available states to explore
    std::vector<S> remaining = {startingState};

    // Keep track of states already evaluated
    std::vector<S> evaluated = {};
    
    // Map of which action led to which thought state
    // Map<future State, Pair<previous State, Action taken>>
    std::unordered_map<S, std::pair<S, A>> history;

    // How much decision power gained so far in each state
    std::unordered_map<S, C> gScore;
    gScore[startingState] = minimumCost;

    // Map of predicted decision power from current state
    std::unordered_map<S, C> fScore;
    fScore[startingState] = heuristic(startingState);

    // Keep processing until there are no states left to check
    while (remaining.size() > 0) {

      // Get the highest priority state to operate on
      auto current = std::min_element(remaining.begin(), remaining.end(),
          [&fScore, &maximumCost, &compareCost](const S& a, const S& b) {

        // Find fScores of a and b
        const C& fA = fScore.find(a) != fScore.end() ?
            fScore[a] : maximumCost;
        const C& fB = fScore.find(b) != fScore.end() ?
            fScore[b] : maximumCost;

        // Return whether fA should come before fB
        return compareCost(fA, fB);
      });

      // If we've arrived at a node that can be considered the goal, stop
      const S state = *current;
      if (isStateGoal(startingState, state)) {

        // Reconstruct the processes taken to get here
        S pathNode = state;
        std::stack<A> actionsTaken;

        // Build the path of actions from finish to start
        while (!(pathNode == startingState)) {

          // Find how we got to the current state
          const auto it = history.find(pathNode);
          if (it != history.end()) {

            // Get the data contained in the kvp
            const auto previous = it->second;

            // Focus on the previous state for next iteration
            pathNode = previous.first;

            // Add action to the stack
            actionsTaken.push(previous.second);
          }

          // This shouldn't trigger as all routes should be traceable
          // However, if it does, stop the infinite loop
          else {
            assert(false);
            return std::make_pair(false, actionsTaken);
          }
        }
        return std::make_pair(true, actionsTaken);
      }

      // No longer consider the current state
      evaluated.insert(evaluated.end(), state);
      remaining.erase(current);

      // Initialise gScore of state if it's not there
      if (gScore.find(state) == gScore.end()) {
        gScore[state] = maximumCost;
      }
      
      // Get possible (neighbour) states by trying all possible actions
      const std::vector<A> actions = getPossibleActions(state);
      std::vector<std::pair<S, A>> states;
      std::for_each(actions.begin(), actions.end(),
          [&states, &evaluated, &state, &takeAction] 
              (const A& action) {

        // Try taking the action with the current state
        const auto attempt = takeAction(state, action);

        // Consider any valid states that haven't been evaluated
        if (attempt.first 
            && std::find(evaluated.begin(), evaluated.end(), attempt.second)
                == evaluated.end()) {
          states.insert(states.end(), std::make_pair(attempt.second, action));
        }
      });

      // For each neighbour
      std::for_each(states.begin(), states.end(),
          [&remaining, &history, &gScore, &fScore, &state, &maximumCost, 
              &weighAction, &heuristic] (const std::pair<S, A>& future) {

        // Initialise gScore of neighbour if it's not there
        if (gScore.find(future.first) == gScore.end()) {
          gScore[future.first] = maximumCost;
        }

        // Work out cost of taking this action with the current state
        const C tentative_gScore = gScore[state] + 
            weighAction(state, future.first, future.second);

        // If our projected score is better than the one recorded
        if (tentative_gScore < gScore[future.first]) {

          // Record how we got to this node (insert new state and the action)
          history.insert(std::make_pair(
                future.first, 
                std::make_pair(state, future.second)));

          // Update the gScore to the lower score
          gScore[future.first] = tentative_gScore;

          // Update fScore to a more accurate representation
          fScore[future.first] = tentative_gScore + heuristic(future.first);

          // If the current future isn't queued to be evaluated next, add it
          if (std::find(remaining.begin(), remaining.end(), future.first) 
              == remaining.end()) {
            remaining.insert(remaining.end(), future.first);
          }
        }
      });
    }

    // Return unsuccessfully with the current state
    return std::make_pair(false, std::stack<A>());
  }
}

#endif
