// TicTacToe.h
// A scene for testing pathfinding with a game of tic-tac-toe

#ifndef TICTACTOE_H
#define TICTACTOE_H

#include "../../Scene.h"

// Basic game of tic-tac-toe
class TicTacToeScene : public Scene {

  // When the tic-tac-toe screen is shown
  void showScene() override {
    Scene::showScene();
  }

  void update(const sf::Time& dt) override {
    Scene::update(dt);
  }
};

#endif
