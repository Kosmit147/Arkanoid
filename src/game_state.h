#pragma once

#include <stdbool.h>

typedef struct GameState
{
    // after adding new fields update resetState()
    unsigned int level;
    bool ballLaunched;
    unsigned int points;
} GameState;

static inline void initGameState(GameState* state, unsigned int level)
{
    state->level = level;
    state->ballLaunched = false;
    state->points = 0;
}

static inline void gameStateAdvanceLevel(GameState* state)
{
    state->level++;
    state->ballLaunched = false;
}
