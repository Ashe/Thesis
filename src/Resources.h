// Resources.h
// Class dedicated to the storage and release of resources

#ifndef RESOURCES_H
#define RESOURCES_H

#include <filesystem>
#include <string>
#include <cctype>
#include <memory>
#include <map>

#include <SFML/Graphics.hpp>

// Forward declaration
class Scene;

class Resources {
  public:

    // Load initial, necessary resources
    void load();

    // Attempt to retrieve a Scene
    Scene* const getScene(const std::string& id) const;

    // Attempt to retrieve a texture
    sf::Texture* const getTexture(const std::string& id) const;

    // Attempt to retrieve a texture
    sf::Font* const getFont(const std::string& id) const;
    
    // Release resources via going out of scope
    void release();

  private:

    // Collection of all scenes
    std::map<std::string, std::unique_ptr<Scene>> scenes_;

    // Collection of textures
    std::map<std::string, std::unique_ptr<sf::Texture>> textures_;

    // Collection of fonts
    std::map<std::string, std::unique_ptr<sf::Font>> fonts_;
};

#endif
