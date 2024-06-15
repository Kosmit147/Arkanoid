/// @file game_state.h
/// @brief GameState structures and functions to operate on it.

#pragma once

#include <stdbool.h>

/// Represents the state of the game like the current level, points scored etc.
typedef struct GameState
{
    unsigned int level;
    bool gameStarted;
    bool ballLaunched;
    bool boardCleared;
    bool gameOver;
    unsigned int points;
} GameState;

/// @brief Initializes a GameState object.
/// @param state Pointer to the GameState object, which will be initialized by the function.
/// @param level Starting level.
static inline void initGameState(GameState* state, unsigned int level)
{
    state->level = level;
    state->gameStarted = false;
    state->ballLaunched = false;
    state->boardCleared = false;
    state->gameOver = false;
    state->points = 0;
}

/// @brief Advances the level in a GameState object.
/// @param state Pointer to the GameState object.
static inline void gameStateAdvanceLevel(GameState* state)
{
    state->level++;
    state->ballLaunched = false;
    state->boardCleared = false;
    state->gameOver = false;
}
