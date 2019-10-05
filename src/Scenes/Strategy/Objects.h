// Strategy/Objects.h
// A collection of classes representing the pieces in play

#ifndef STRATEGY_OBJECTS_H
#define STRATEGY_OBJECTS_H

// Seperate Strategy related classes from other games
namespace Strategy {

  // Represent pawns as an enum
  enum class Object {
    Nothing,
    Wall,
    CHARACTERS,
    Bazooka,
    Machinegunner,
    Shotgunner
  };
}

#endif
