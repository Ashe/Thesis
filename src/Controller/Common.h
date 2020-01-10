// Controller/Common.h
// Definition of common types and functions for working with controllers

#ifndef CONTROLLER_COMMON_H
#define CONTROLLER_COMMON_H

// Keep dependencies to a minimum
#include <string>

// Encapsulate controller types and functions
namespace Controller {

  // Type of controller
  static const char* typeList[] = {
      "Human", "Idle", "Random", "AStarOne", "AStarTwo", "AStarThree"};
  enum class Type {
    Human,
    Idle,
    Random,
    AStarOne,
    AStarTwo,
    AStarThree,
    COUNT
  };

  // Convert controller enum to string
  inline std::string typeToString(const Controller::Type& controller) {
    return typeList[(int)controller];
  }
}

#endif
