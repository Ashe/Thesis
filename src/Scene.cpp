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

// When the screen is shown
void
Scene::showScene() {

  // Begin the scene if it hasn't yet
  if (!hasBegun_) {
    onBegin();
    hasBegun_ = true;
  }
  onShow();
}

// When the screen is hidden
void
Scene::hideScene() {
  onHide();
}

// When the scene is about to be quit
void
Scene::quit() {
  Console::log("Quitting scene..");
  onQuit();
  App::terminate();
}

// Called when the scene is started
void
Scene::onBegin() {}

// Update the app every frame
void
Scene::onUpdate(const sf::Time& dt) {}

// Render the app every frame
void
Scene::onRender(sf::RenderWindow& window) {}

// Handle keypresses
void
Scene::onEvent(const sf::Event& event) {}

// Logic to perform when showing the scene
void
Scene::onShow() {}

// Logic to perform when hiding the scene
void
Scene::onHide() {}

// When the app has requested to quit
void
Scene::onQuit() {
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
