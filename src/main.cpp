// main.cpp
// Entry point of program

// NOTE: This file has been repurposed from my other project from:
// https://github.com/Ashe/Relocate-Engine/blob/master/src/main.cpp

#include "App.h"

#ifdef linux
#include <X11/Xlib.h>
#endif

// Create and start the game
int main(int argc, char* argv[]) {

  // Should the program be run in debug mode?
  bool debug = true;

  // Should print statements go to terminal as well as console?
  bool outputToTerminal = true;

  // Set up whether we should multi thread or not
  bool multiThread = true, multiThreadSuccess = false;

#ifdef WIN32
  // Multithreading 'just works' on Windows
  multiThreadSuccess = true;
#endif

// @TODO: More investigation is required to see if this will work 
// on all Linux distros
#ifdef linux
  const int i = XInitThreads();
  if (i != 0) { multiThreadSuccess = true; }
  else { printf("Error: Failed to call XInitThreads, code %d\n", i); }
#endif

  // Initialise and start the game
  App::initialise(
      "App", 
      sf::VideoMode(1280, 720), 
      multiThread && multiThreadSuccess,
      debug,
      outputToTerminal);
  App::switchScene("welcome");
  App::start();
  App::shutdown();
  return 0;
}
