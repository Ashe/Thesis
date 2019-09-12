// Resources.h
// Class dedicated to the storage and release of resources

#ifndef RESOURCES_H
#define RESOURCES_H

#include <memory>
#include <map>

// Forward declaration
class Scene;

class Resources {
  public:

    // Load initial, necessary resources
    void load();

    // Add a new Scene to be managed
    void addScene(const std::string& id, std::unique_ptr<Scene> scene);

    // Retrieve a Scene
    Scene* const getScene(const std::string& id);
    
    // Release resources via going out of scope
    void release();

  private:

    // Collection of all scenes
    std::map<std::string, std::unique_ptr<Scene>> scenes_;

};

#endif
