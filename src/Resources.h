// Resources.h
// Class dedicated to the storage and release of resources

#ifndef RESOURCES_H
#define RESOURCES_H

#include <filesystem>
#include <fstream>
#include <string>
#include <sstream>
#include <cctype>
#include <memory>
#include <map>
#include <set>

#include <SFML/Graphics.hpp>

// Forward declaration
class Scene;
struct Map;

class Resources {
  public:

    // Load initial, necessary resources
    void load();

    // Retrieve all map names
    std::set<std::string> getStratMapIds() const;

    // Attempt to retrieve a Scene
    Scene* const getScene(const std::string& id) const;

    // Attempt to retrieve a texture
    sf::Texture* const getTexture(const std::string& id) const;

    // Attempt to retrieve a texture
    sf::Font* const getFont(const std::string& id) const;

    // Attempt to get a strategy map
    std::string getStrategyMapString(const std::string& id) const;
    
    // Release resources via going out of scope
    void release();

  private:

    // Collection of all scenes
    std::map<std::string, std::unique_ptr<Scene>> scenes_;

    // Collection of textures
    std::map<std::string, std::unique_ptr<sf::Texture>> textures_;

    // Collection of fonts
    std::map<std::string, std::unique_ptr<sf::Font>> fonts_;

    // Collection of strategy mapstrings
    std::map<std::string, std::string> strategyMaps_;
};

#endif
