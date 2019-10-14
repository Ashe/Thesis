// App.cpp
// This is a static, high level manager for the app's cycle

#include "App.h"
#include "Scene.h"

// Initialise static members
sf::RenderWindow* App::window_ = nullptr;
sf::View App::view = sf::View();
bool App::multiThread_ = false;
std::mutex App::windowMutex_;
bool App::debug_ = false;
App::Status App::status_ = App::Status::Uninitialised;
Resources App::resources_ = Resources();
Scene* App::currentScene_ = nullptr;
sf::Vector2f App::mousePosition_ = sf::Vector2f();
sf::Vector2f App::displaySize_ = sf::Vector2f();
Console App::console_;
bool App::showConsole_ = false;
unsigned App::fps_ = 0;
bool App::isImguiReady_ = false;
std::queue<ImWchar> App::queuedChars_ = std::queue<ImWchar>();

// Initialise the app without starting the loop
void
App::initialise(
    const std::string& title, 
    const sf::VideoMode& mode, 
    bool multiThread,
    bool enableDebugMode,
    bool outputToTerminal) {

  // Enable debug mode
  App::debug_ = enableDebugMode;

  // Enable console debugging
  Console::initialise(outputToTerminal);

  // Check that we haven't already initialised
  if (status_ != App::Status::Uninitialised) {
    Console::log("[Error] Cannot initialise application - already running."); 
    return;
  }

  // Print opening message
  Console::log("Launching Application...");

  // Flag whether we are in multithreaded mode
  multiThread_ = multiThread;

  // Print if we are in multithreaded mode or not
  Console::log("Running in %s mode.", 
      multiThread_ ? "multithreaded" : "standard");

  // Create window and prepare view
  window_ = new sf::RenderWindow(mode, title);
  view = window_->getDefaultView();

  // Set up size of the window
  const auto size = window_->getSize();
  displaySize_ = sf::Vector2f(size.x, size.y);

  // Load assets
  resources_.load();

  // Enable debugging functionality
  ImGui::SFML::Init(*window_);

  // Flag that the app is ready to start
  status_ = App::Status::Ready;
}

// Initiate the app loop
void
App::start() {

  // Check that the app is ready to start
  if (status_ != App::Status::Ready) {
    Console::log("[Error] Cannot start application."); 
    return;
  }

  // We are now running the app
  status_ = App::Status::Running;

  // Create a clock for measuring deltaTime
  sf::Clock clock_;

  // FPS variables
  sf::Clock fpsClock_;
  unsigned fpsFrame_ = 0;

  // Disable the window
  if (multiThread_) {
    window_->setActive(false);
  }

  // Pointer to the render thread if necessary
  std::thread* renderThread;

  // Start a thread to render the app
  if (multiThread_) {
    renderThread = new std::thread(App::handleRenderThread);
    renderThread->detach();
  }

  // Main app loop while window is open
  while (status_ < App::Status::ShuttingDown) {

    // Get the time since last tick
    sf::Time elapsed_ = clock_.restart();

    // Calculate FPS
    if (fpsClock_.getElapsedTime().asSeconds() >= 1.0f) {
      fps_ = fpsFrame_;
      fpsFrame_ = 0;
      fpsClock_.restart();
    }
    ++fpsFrame_;

    // Prepare to collect input events
    bool process = true;
    std::vector<sf::Event> events;

    // If multithreading, gain access to window quickly
    if (multiThread_) { process = windowMutex_.try_lock(); }
    sf::Event e;

    // If we're allowed to process data
    if (process) {

      // Collect events
      while (window_->pollEvent(e)) {
        events.push_back(e);
      }

      // Disable the lock, we don't need the window
      if (multiThread_) { 
        windowMutex_.unlock(); 
      }

      // Resume processing events
      for (auto ev : events) {

        // Begin closing the app on quit
        // Otherwise, pass the event to the scene
        if (ev.type != sf::Event::Closed) {
          handleEvent(ev);
        }
        else {
          quit();
        }
      }
    }

    // Update the app
    update(elapsed_);

    // Render every frame after updating
    if (!multiThread_) {
      if (status_ < App::Status::ShuttingDown) {
        render();
      }
    }
    else {
      std::this_thread::yield();
    }
  }

  // Wait for render thread to finish
  if (multiThread_) {
    if (renderThread->joinable()) { renderThread->join(); }
    delete renderThread;
  }
}

// Called every frame, returns true when app should end
void
App::update(const sf::Time& dt) {

  // Easy out
  if (window_ == nullptr) return;

  // Update mouse position every frame
  sf::Vector2i mousePixelCoords = sf::Mouse::getPosition(*window_);
  mousePosition_ = window_->mapPixelToCoords(mousePixelCoords);

  // Update the screen if the pointer is set
  if (currentScene_ != nullptr) {
    currentScene_->onUpdate(dt);
  }

  // Update IMGUI debug interfaces
  if (debug_ && !isImguiReady_) {

    // Pass queued characters to ImGui
    auto& io = ImGui::GetIO();
    while (!queuedChars_.empty()) {
      io.AddInputCharacter(queuedChars_.front());
      queuedChars_.pop();
    }

    // Update imgui (calls ImGui::NewFrame())
    ImGui::SFML::Update(mousePixelCoords, displaySize_, dt);

    // Create imgui interfaces
    App::handleImgui();

    // Ready the IMGUI frame
    isImguiReady_ = true;
  }
}

// Call the render() function from a seperate thread
void
App::handleRenderThread() {

  // Easy out
  if (App::getStatus() < App::Status::Running) return;

  // Continually render the app while
  while (App::getStatus() < App::Status::ShuttingDown) {
    windowMutex_.lock();
    App::render();
    windowMutex_.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(3));
  }
}

// Render the app every frame, after update
void
App::render() {

  // Easy out
  if (window_ == nullptr) return;

  // Clear the window for rendering
  window_->clear();

  // Render the app if pointer is set
  if (currentScene_ != nullptr) {
    window_->setView(view);
    currentScene_->onRender(*window_);
  }

  // Render IMGUI debug interface
  if (isImguiReady_) {
    ImGui::SFML::Render(*window_);
    isImguiReady_ = false;
  }

  // Render everything in the screen
  window_->display();
}

// Respond to any key or mouse related events
void
App::handleEvent(const sf::Event& event) {

  // Adjust the viewport if window is resized
  if (event.type == sf::Event::Resized) {
    displaySize_ = sf::Vector2f(
        event.size.width, 
        event.size.height);
    sf::FloatRect visibleArea(
        0.f, 0.f, 
        displaySize_.x, displaySize_.y);
    App::view = sf::View(visibleArea);
  }

  // Pass events to IMGUI debug interface
  bool passToGame = true;
  bool passToImgui = debug_;
  if (debug_) {

    // Get reference to IO
    auto& io = ImGui::GetIO();

    // Decide whether the app should get the event
    if (event.type == sf::Event::MouseButtonPressed || 
        event.type == sf::Event::MouseButtonReleased) {
      if (io.WantCaptureMouse) passToGame = false;
    }
    else if ( event.type == sf::Event::KeyPressed || 
              event.type == sf::Event::KeyReleased) {
      if (io.WantCaptureKeyboard) passToGame = false;
    }
    else if (event.type == sf::Event::TextEntered 
        && io.WantTextInput) {
      passToImgui = false;
      auto imguiChar = static_cast<ImWchar>(event.text.unicode);
      queuedChars_.push(imguiChar);
    }
  }

  // Handle imgui events
  if (passToImgui) { ImGui::SFML::ProcessEvent(event); }

  // Pass events to scene
  if (currentScene_ != nullptr && passToGame) {
    currentScene_->onEvent(event);
  }
}

// Release resources before the app closes
void
App::quit() {

  // Notify application that quit was requested
  Console::log("Attempting to quit application..");

  // Signal that we're quitting the app
  status_ = App::Status::Quitting;

  // Allow the current screen to halt termination
  if (currentScene_) {
    currentScene_->quit();
  }
  else {
    terminate();
  }
}

// Called from within application to close it down
void
App::terminate() {

  // Notify app that we're terminating
  Console::log("Terminating application..");

  // Set the app's status to break game loop
  status_ = App::Status::ShuttingDown;

  // Close the window, exiting the app loop
  if (window_ != nullptr) {
    window_->close();
    delete window_;
    window_ = nullptr;
  }

  // Shut down IMGUI debug interface
  ImGui::SFML::Shutdown();
}

// Free resources before program closes
void
App::shutdown() {

  // Reset status
  status_ = App::Status::Uninitialised;

  // Resources are released via the App going out of scope
  // But you can force it which may be desired when calling shutdown
  resources_.release();

  // Shut down console debugging
  Console::shutdown();
}

// Change to the new screen
bool 
App::switchScene(const std::string& sceneID) {

  // Log what we're attempting
  Console::log("Switching to scene: %s..", sceneID.c_str());

  // Try to retrieve a scene from resources
  auto scene = resources_.getScene(sceneID);

  // Easy out if scene doesn't exist
  if (scene == nullptr) {
    Console::log(".. failed.");
    return false;
  }

  // Switch away from old screen
  if (currentScene_ != nullptr) {
    currentScene_->hideScene();
  }

  // Change the screen to be rendered
  currentScene_ = scene;

  // Run any logic for showing screen
  if (currentScene_ != nullptr) {
    currentScene_->showScene();
  }

  // Declare that the scene switch succeeded
  return true;
}

// Update imgui interfaces
void
App::handleImgui() {

  // Set up IMGUI flags
  ImGuiWindowFlags flags = 0;
  flags |= ImGuiWindowFlags_MenuBar;

  // Standard debug info
  // @TODO: Remove call to imgui demo
  static bool showImguiDemo = false;
  ImGui::Begin("Debug", NULL, flags);
  if (ImGui::BeginMenuBar()) {

    // Show scene switcher
    if (ImGui::BeginMenu("Scenes")) {
      bool showWelcome = false;
      bool showTicTacToe = false;
      bool showStrategy = false;
      ImGui::MenuItem("Welcome", NULL, &showWelcome);
      ImGui::MenuItem("Tic-Tac-Toe", NULL, &showTicTacToe);
      ImGui::MenuItem("Strategy", NULL, &showStrategy);
      ImGui::EndMenu();

      // Change scenes if user clicks the buttons
      if (showWelcome) switchScene("welcome");
      else if (showTicTacToe) switchScene("ticTacToe");
      else if (showStrategy) switchScene("strategy");
    }

    // Show different tools
    if (ImGui::BeginMenu("View")) {
      ImGui::MenuItem("Demo imgui", NULL, &showImguiDemo);
      ImGui::MenuItem("Console", NULL, &showConsole_);

      // Allow the scene to make entries to the view tab
      if (currentScene_ != nullptr) {
        currentScene_->addDebugMenuEntries();
      }
      ImGui::EndMenu();
    }

    ImGui::EndMenuBar();
  }

  // Show ImGui demo on request
  if (showImguiDemo) { ImGui::ShowDemoWindow(&showImguiDemo); }

  // Show console
  if (showConsole_) { console_.create("Console", &showConsole_); }

  // Info
  ImGui::Spacing();
  ImGui::Text("FPS: %u", fps_);
  ImGui::Text("Window Size: %d x %d",
      (int)displaySize_.x,
      (int)displaySize_.y);

  // Camera
  const auto viewCentre = view.getCenter();
  ImGui::Text("Camera Position: (%d, %d)",
      (int)viewCentre.x,
      (int)viewCentre.y);

  // Mouse
  ImGui::Text("Mouse Position: (%d, %d)",
      (int)mousePosition_.x,
      (int)mousePosition_.y);
  ImGui::Spacing();

  // End default debug window
  ImGui::End();

  // Allow scene to make debug windows
  if (currentScene_ != nullptr) {
    currentScene_->addDebugDetails();
  }
}

// Open the dev console in the terminal
void
App::openDevConsole() {
  showConsole_ = !showConsole_;
  if (showConsole_ && !debug_) {
    debug_ = true;
  }
}

// Get a pointer to the app's window
const sf::RenderWindow*
App::getWindow() {
  return window_;
}

// Get FPS of application
unsigned
App::getFPS() {
  return fps_;
}

// Get status of application
App::Status
App::getStatus() {
  return status_;
}

// Get app resources
Resources& 
App::resources() {
  return resources_;
}

// Get up to date mouse position
sf::Vector2f
App::getMousePosition() {
  return mousePosition_;
}

// Get up to display size
sf::Vector2f
App::getDisplaySize() {
  return displaySize_;
}

// Set debug mode
void
App::setDebugMode(bool enable) {
  if (debug_) { Console::log("Debug mode %s.", enable ? "enabled" : "disabled"); }
  debug_ = enable;
  if (debug_) { Console::log("Debug mode %s.", enable ? "enabled" : "disabled"); }
}

// Get debug mode
bool
App::getDebugMode() {
  return debug_;
}
