/// @file game.h
/// @brief Functions used to initialize the game, update it and free it.

#pragma once

#include "game_state.h"
#include "board.h"
#include "rendering.h"

/// @brief Structure which contains all of the data required to run the game.
typedef struct Game
{
    GameState state;
    Board board;
    Renderer renderer;
} Game;

/// @brief Returns whether the game is over.
/// @return True if game over, false otherwise.
static inline bool gameOver(const Game* game) { return ballOutOfBounds(&game->board.ball); }

/// @brief Initializes the game.
/// @param game Pointer to the Game object which will be initialized by the function.
/// @param level Starting level.
void initGame(Game* game, unsigned int level);
/// @brief Advances the level.
/// @param game Pointer to the game object.
void advanceLevel(Game* game);
/// @brief Frees the game data.
/// @param game Pointer to the game object.
void freeGame(Game* game);

/// @brief Processes game input. Updates the paddle position or starts/restarts the game if the right key was
/// pressed.
/// @param game Pointer to the game object.
/// @param window Pointer to the window. Needed for input detection.
void processGameInput(Game* game, GLFWwindow* window);
/// @brief Moves game objects (the ball).
/// @param Pointer to the game object.
void moveGameObjects(Game* game);
/// @brief Collides game objects and removes them from rendering data.
/// @param Pointer to the game object.
void collideGameObjects(Game* game);
