// Resources.cpp
// Class dedicated to the storage and release of resources

#include "Resources.h"

// Avoid cyclic dependencies by including in cpp file
#include "Scenes/Welcome/Welcome.h"
#include "Scenes/TicTacToe/TicTacToe.h"

// Load initial, necessary resources
void
Resources::load() {
  addScene("welcome", std::make_unique<WelcomeScene>());
  addScene("ticTacToe", std::make_unique<TicTacToeScene>());
}

// Add a new Scene to be managed
void 
Resources::addScene(const std::string& id, std::unique_ptr<Scene> scene) {
  scenes_.emplace(id, std::move(scene));
}

// Retrieve a Scene
Scene* const 
Resources::getScene(const std::string& id) {
  auto it = scenes_.find(id);
  if (it != scenes_.end()) {
    return it->second.get();
  }
  return nullptr;
}

// Release resources
void
Resources::release() {
  scenes_.clear();
}
