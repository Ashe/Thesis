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
      "Human", "Idle", "Random", "AStarOne", "AStarTwo"};
  enum class Type {
    Human,
    Idle,
    Random,
    AStarOne,
    AStarTwo
  };

  // Convert controller enum to string
  inline std::string typeToString(const Controller::Type& controller) {
    return typeList[(int)controller];
   //switch(controller) {
   //  case Type::Human: return "Human"; break;
   //  case Type::Random: return "Random"; break;
   //  case Type::AStarOne: return "AStar(1)"; break;
   //  default: return "UNKNOWN"; break;
   //}
  }
}

#endif
