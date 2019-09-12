// App.h
// This is a static, high level manager for the app's cycle

#ifndef APP_H
#define APP_H

#include <thread>
#include <mutex>
#include <chrono>
#include <functional>
#include <queue>

#include <SFML/Graphics.hpp>

#include "Console.h"

// Forward declaration
class Scene;

// Static class containing the application
class App {
  public:

    // What the app is currently doing
    enum Status { Uninitialised, Ready, Running, Quitting, ShuttingDown };

    // View to use for rendering
    static sf::View view;

    // Initialise the app GUI window
    static void initialise(
        const std::string& title,
        const sf::VideoMode& m,
        bool multiThread = false,
        bool enableDebugMode = false,
        bool outputToTerminal = false);

    // Start the app, calling update and render loops
    static void start();

    // Change the screen that is used and rendered
    static void switchScene(Scene* scene);

    // Get a pointer to const window
    static const sf::RenderWindow* getWindow();

    // Get the FPS of the application
    static unsigned getFPS();

    // Get the status of the app
    static Status getStatus();

    // Open the dev console
    static void openDevConsole();

    // Tell the application to quit
    static void quit();

    // Signal that the application can now close safely
    static void terminate();

    // Release resources before closing
    // @NOTE: Call from wherever app is started
    static void shutdown();

    // Get the up to date mouse position
    static sf::Vector2f getMousePosition();

    // Get the up to date mouse position
    static sf::Vector2f getDisplaySize();

    // Debug mode
    static bool getDebugMode();
    static void setDebugMode(bool enable);

  private:

    // Tracking what the app is currently doing
    static Status status_;

    // Whether we are multithreaded
    static bool multiThread_;

    // Enable debugging functionality
    static bool debug_;

    // Window to render to
    static sf::RenderWindow* window_;

    // Mutex to protect the window for rendering
    static std::mutex windowMutex_;

    // Scene management
    static Scene* currentScene_;

    // Up to date mouse position
    static sf::Vector2f mousePosition_;

    // Up to date display size
    static sf::Vector2f displaySize_;

    // Console for debugging
    static Console console_;
    static bool showConsole_;

    // FPS
    static unsigned fps_;

    // Whether IMGUI is ready
    static bool isImguiReady_;

    // Queued characters
    static std::queue<ImWchar> queuedChars_;

    // Main app loop 
    static void update(const sf::Time& dt);

    // Multithread's render loop
    static void handleRenderThread();

    // Render the app
    static void render();

    // Handle keypresses etc
    static void handleEvent(const sf::Event& event);

    // Handle the updating of IMGUI interfaces
    static void handleImgui();
};

#endif
