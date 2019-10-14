// Resources.cpp
// Class dedicated to the storage and release of resources

#include "Resources.h"

// Avoid cyclic dependencies by including in cpp file
#include "Scenes/Welcome/Welcome.h"
#include "Scenes/TicTacToe/TicTacToe.h"
#include "Scenes/Strategy/Strategy.h"

// Load initial, necessary resources
void
Resources::load() {

  // Populate scene list with instances of Scenes
  scenes_.emplace("welcome", std::make_unique<WelcomeScene>());
  scenes_.emplace("ticTacToe", std::make_unique<TicTacToe::Game>());
  scenes_.emplace("strategy", std::make_unique<Strategy::Game>());

  // Declare that we're loading resources
  const std::string dir = "Assets/";
  Console::log("Loading resources recursively from directory: '%s'..", 
    dir.c_str());

  // For every file in the directory
  for (const auto& entry : 
      std::filesystem::recursive_directory_iterator(dir)) {
    const auto fp = entry.path();
    if (entry.is_regular_file()) {// && fp.extension().string() == ".lua") {

      // Prepare to load resources
      bool loaded = false;

      // Get the extension to load the resource properly
      auto ext = fp.extension().string();
      std::transform(ext.begin(), ext.end(), ext.begin(),
          [](unsigned char c) { return std::tolower(c); });

      // Textures
      if (ext == ".png" || ext == ".jpg" || ext == ".jpeg") {
        auto tex = std::make_unique<sf::Texture>();
        if (tex->loadFromFile(fp)) {
          textures_.emplace(fp.stem(), std::move(tex));
          Console::log("Loaded texture: %s as %s", 
              fp.c_str(), fp.stem().c_str());
          loaded = true;
        }
      }

      // Fonts
      else if (ext == ".ttf") {
        auto font = std::make_unique<sf::Font>();
        if (font->loadFromFile(fp)) {
          fonts_.emplace(fp.stem(), std::move(font));
          Console::log("Loaded font: %s as %s",
              fp.c_str(), fp.stem().c_str());
          loaded = true;
        }
      }

      // Strategy maps
      else if (ext == ".stratmap") {
        std::ifstream infile(fp);
        std::stringstream ss;
        ss << infile.rdbuf();
        strategyMaps_.emplace(fp.stem(), ss.str());
        Console::log("Loaded strategy map: %s as %s",
            fp.c_str(), fp.stem().c_str());
        loaded = true;
      }

      // If resource failed to load
      if (!loaded) {
        Console::log("[Error] Failed to load resource: %s", fp.c_str());
      }
    }
  }
}

// Retrieve all map names
std::set<std::string> 
Resources::getStratMapIds() const {
  std::set<std::string> ids;
  for (const auto& kvp : strategyMaps_) {
    ids.insert(kvp.first);
  }
  return ids;
}

// Attempt to retrieve a Scene
Scene* const 
Resources::getScene(const std::string& id) const {
  auto it = scenes_.find(id);
  if (it != scenes_.end()) {
    return it->second.get();
  }
  Console::log("[Error] Unable to retrieve scene: %s", id.c_str());
  return nullptr;
}

// Attempt to retrieve a Texture
sf::Texture* const
Resources::getTexture(const std::string& id) const {
  auto it = textures_.find(id);
  if (it != textures_.end()) {
    return it->second.get();
  }
  Console::log("[Error] Unable to retrieve texture: %s", id.c_str());
  return nullptr;
}

// Attempt to retrieve a texture
sf::Font* const 
Resources::getFont(const std::string& id) const {
  auto it = fonts_.find(id);
  if (it != fonts_.end()) {
    return it->second.get();
  }
  Console::log("[Error] Unable to retrieve font: %s", id.c_str());
  return nullptr;
}

// Attempt to get a strategy map
std::string 
Resources::getStrategyMapString(const std::string& id) const {
  auto it = strategyMaps_.find(id);
  if (it != strategyMaps_.end()) {
    return it->second;
  }
  Console::log("[Error] Unable to strategy map : %s", id.c_str());
  return std::string();
}

// Release resources
void
Resources::release() {
  scenes_.clear();
}
