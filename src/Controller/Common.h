// Controller/Common.h
// Definition of common types and functions for working with controllers

#ifndef CONTROLLER_COMMON_H
#define CONTROLLER_COMMON_H

// Keep dependencies to a minimum
#include <string>

// Encapsulate controller types and functions
namespace Controller {

  // Type of controller
  static const char* typeList[] = {"Human", "Random", "AStar"};
  enum class Type {
    Human,
    Random,
    AStar,
    COUNT
  };

  // Convert controller enum to string
  inline std::string typeToString(const Controller::Type& controller) {
    switch(controller) {
      case Type::Random: return "Random"; break;
      case Type::AStar: return "AStar"; break;
      default: return "Human"; break;
    }
  }
}

#endif
