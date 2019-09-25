// TicTacToe.h
// A scene for testing pathfinding with a game of tic-tac-toe

#ifndef TICTACTOE_H
#define TICTACTOE_H

#include <math.h>
#include <utility>
#include "../../Scene.h"

#include "../../Controllers/Random/Random.h"
#include "../../Controllers/AStar/AStar.h"

#define BOARDSIZE 3

// Representation of who can play
// @TODO: Factor this out so that other scenes can use it
enum Controller {
  Human,
  Random,
  COUNT
};

// Representation of players
enum Player {
  N,
  X,
  O
};

// Representation of moves
typedef sf::Vector2i Move;

// Simple gamestate structure
struct GameState {

  // Starting player
  static const Player firstPlayer;

  // Turn number (indexed at 1 for ease of reading)
  unsigned int turnNumber = 1;

  // Track who's turn it is
  Player currentTurn = firstPlayer;

  // State of the board
  Player boardState[BOARDSIZE][BOARDSIZE] = {
    { N, N, N },
    { N, N, N },
    { N, N, N }
  };
};

// Basic game of tic-tac-toe
class TicTacToeScene : public Scene {
  public:

    ///////////////////////////////////////////
    // SCENE FUNCTIONS:
    // - Mandatory functions for the scene
    ///////////////////////////////////////////

    // @TODO: Delete this
    void onBegin() override {
      states_.push_back(GameState());
      continueGame();
    }

    // Update the currently hovered tile
    void onUpdate(const sf::Time& dt) override;

    // Handle input and game size changes
    void onEvent(const sf::Event& event) override;

    // Render the render the game board and state
    void onRender(sf::RenderWindow& window) override;

    // Whenever the scene is re-shown, ensure graphics are correct
    void onShow() override;

    // Add details to debug windows
    void addDebugDetails() override;

  private:

    // Tracker for which state to view
    unsigned int currentState_;

    // States of the game
    std::vector<GameState> states_;

    // Has the game been won at some point?
    bool isGameOver_;
    Player winner_;

    // Who is playing who
    Controller playerX_ = Controller::Human;
    Controller playerO_ = Controller::Human;

    // Player colours
    sf::Color playerXColour_ = sf::Color(0, 117, 252);
    sf::Color playerXColourHovered_ = sf::Color(0, 0, 130);
    sf::Color playerOColour_ = sf::Color(255, 0, 0);
    sf::Color playerOColourHovered_ = sf::Color(130, 0, 0);

    // Player icons
    sf::VertexArray playerIconX_;
    sf::CircleShape playerIconO_;

    // The tic-tac-toe board
    sf::VertexArray board_;

    // The mouse hovered tile
    sf::Vector2i mouseTile_;

    // Key dimensions
    float gameSize_;
    float tileSize_;

    // Key Positions
    sf::Vector2f center_;
    float top_, left_, right_, bottom_;

    ///////////////////////////////////////////
    // PURE FUNCTIONS:
    // - Functions without side effects
    // - Used to transform or read game states
    ///////////////////////////////////////////
    
    // Get a collection of valid moves one could make
    static std::vector<Move> getValidMoves(const GameState& state);

    // Check if the game has been won by a player
    // Returns (isGameOver, winner)
    static std::pair<bool, const Player> checkGameover(const GameState& state);

    // Attempts to make the move on the game state and returns new state
    // Returns (isStateValid, newState)
    static std::pair<bool, const GameState> makeMove(
        const GameState& state, 
        const Move& move);

    // Check if a move is valid
    static bool isValidMove(const Move& move);
  
    ///////////////////////////////////////////
    // IMPURE FUNCTIONS:
    // - Mutate the state of the scene
    ///////////////////////////////////////////
    
    // Recursive function for checking and performing AI moves
    void continueGame();

    // Reset's the state of tic-tac-toe back to the beginning
    void resetGame();

    // Get a gamestate safely
    std::pair<bool, const GameState> getState(unsigned int n) const;

    // Check which controller is currently playing
    Controller getControllerOfCurrentPlayer(const Player& player) const;

    ///////////////////////////////////////////
    // GRAPHICAL / LOGGING:
    // - Non-logic pure or impure functions
    ///////////////////////////////////////////

    // Adjust graphics for current game size
    void resizeGame();

    // Draw a specific gamestate
    void drawGameState(sf::RenderWindow& window, const GameState& state);

    // Draw an icon in a tile
    void drawIcon(
        sf::RenderWindow& window, 
        Move move,
        Player player,
        bool hovered = false);

    // Log to console the move that was just performed
    void logMove(int stateNo, Player currentTurn, Move move) const;

    // Get a string of the player
    static std::string getPlayerAsString(const Player& player);

    // Get a string of the kind of controller
    static std::string getControllerAsString(const Controller& controller);
};

#endif
