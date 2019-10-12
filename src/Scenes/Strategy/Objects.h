// Strategy/Objects.h
// A collection of classes representing the pieces in play

#ifndef STRATEGY_OBJECTS_H
#define STRATEGY_OBJECTS_H

#include "Common.h"

// Seperate Strategy related classes from other games
namespace Strategy {

  // Represent pawns as an enum
  enum class Object {
    Nothing,
    Wall,
    MeleeUnit,
    BlasterUnit,
    SniperUnit,
    LaserUnit
  };

  // Check for unit
  inline bool isUnit(const Object& o) {
    return o >= Object::MeleeUnit;
  }

  // Get MP cost per move
  inline Points getUnitMPCost(const Object& o) {
    switch (o) {
      case Object::MeleeUnit: return Points(1); break;
      case Object::BlasterUnit: return Points(2); break;
      case Object::SniperUnit: return Points(2); break;
      case Object::LaserUnit: return Points(3); break;
      default: return Points(0); break;
    }
  }

  // Get AP cost per attack
  inline Points getUnitAPCost(const Object& o) {
    switch (o) {
      case Object::MeleeUnit: return Points(1); break;
      case Object::BlasterUnit: return Points(1); break;
      case Object::SniperUnit: return Points(2); break;
      case Object::LaserUnit: return Points(3); break;
      default: return Points(0); break;
    }
  }

  // Get the range of a unit
  inline Range getUnitRange(const Object& o) {
    switch (o) {
      case Object::MeleeUnit: return Range(1); break;
      case Object::BlasterUnit: return Range(3); break;
      case Object::SniperUnit: return Range(10); break;
      case Object::LaserUnit: return Range(25); break;
      default: return Range(0); break;
    }
  }
}

#endif
