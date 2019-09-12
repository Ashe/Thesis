// Scene.h
// Non-engine oriented logic goes here

#ifndef SCENE_H
#define SCENE_H

#include <string>
#include <memory>
#include <map>

#include "App.h"

// Represents it's own world of objects
class Scene {
  public:

    // Constructors
    Scene();

    // Copy constructor
    Scene(const Scene& other);

    // Destructor
    ~Scene();

    // Update the app every frame
    virtual void update(const sf::Time& dt);

    // Perform any rendering
    virtual void render(sf::RenderWindow& window);

    // Handle app input
    virtual void handleEvent(const sf::Event& event);

    // Perform logic when shown or hidden
    virtual void showScene();
    virtual void hideScene();

    // Add a menu entry to the debug menu
    virtual void addDebugMenuEntries();

    // Add functionality to the default window
    virtual void addDebugDetails();

    // Called when user closes window or you call App::quit()
    // DO NOT call this, this is called by App.h
    // This MUST call App::terminate() to safely close the app
    virtual void quit();

  protected:

    // Called when first shown before update
    virtual void begin();

  private:

    // If the scene has begun
    bool hasBegun_;
};

#endif
