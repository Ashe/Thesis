// Scene.cpp
// Non-engine oriented logic goes here

#include "Scene.h"

// Constructor
Scene::Scene ()
  : hasBegun_(false) {
}

// Copy constructor
Scene::Scene(const Scene& other) 
  : hasBegun_(other.hasBegun_) {
}

// Destructor
Scene::~Scene() {}

// Called when the scene is started
void
Scene::begin() {

  // Flag that the scene has started
  hasBegun_ = true;
}

// When the screen is shown
void
Scene::showScene() {

  // Begin the scene if it hasn't yet
  if (!hasBegun_) {
    begin();
  }
}

// When the screen is hidden
void
Scene::hideScene() {}

// Update the app every frame
void
Scene::update(const sf::Time& dt) {}

// Render the app every frame
void
Scene::render(sf::RenderWindow& window) {}

// Handle keypresses
void
Scene::handleEvent(const sf::Event& event) {}

// When the app has requested to quit
void
Scene::quit() {
  Console::log("Quitting scene..");
  App::terminate();
}

/////////////////////
// DEBUG FUNCTIONS //
/////////////////////

// Add entries to debug menu
void
Scene::addDebugMenuEntries() {}

// Add information to ImGui windows
void
Scene::addDebugDetails() {}
