// Scene.h
// Non-engine oriented logic goes here

// NOTE: This file has been repurposed from my other project from:
// https://github.com/Ashe/Relocate-Engine/blob/master/src/Scene.h

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
    virtual void onUpdate(const sf::Time& dt);

    // Perform any rendering
    virtual void onRender(sf::RenderWindow& window);

    // Handle app input
    virtual void onEvent(const sf::Event& event);

    // Show the scene
    void showScene();

    // Hide the scene
    void hideScene();

    // Called when user closes window or you call App::quit()
    // DO NOT call this, this is called by App.h
    // This MUST call App::terminate() to safely close the app
    void quit();

    // Add a menu entry to the debug menu
    virtual void addDebugMenuEntries();

    // Add functionality to the default window
    virtual void addDebugDetails();

  protected:

    // Called when first shown before update
    virtual void onBegin();

    // Called when the scene is shown
    virtual void onShow();

    // Called when the scene is hidden
    virtual void onHide();

    // Safe callback for quit() for scene logic
    virtual void onQuit();

  private:

    // If the scene has begun
    bool hasBegun_;
};

#endif
