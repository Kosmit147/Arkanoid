#pragma once

#include <stdbool.h>

typedef struct GameState
{
    unsigned int level;
    bool gameStarted;
    bool ballLaunched;
    bool boardCleared;
    bool gameOver;
    unsigned int points;
} GameState;

static inline void initGameState(GameState* state, unsigned int level)
{
    state->level = level;
    state->gameStarted = false;
    state->ballLaunched = false;
    state->boardCleared = false;
    state->gameOver = false;
    state->points = 0;
}

static inline void gameStateAdvanceLevel(GameState* state)
{
    state->level++;
    state->ballLaunched = false;
    state->boardCleared = false;
    state->gameOver = false;
}
