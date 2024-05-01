#pragma once

#include <stdbool.h>
#include <stdio.h>

#include "defines.h"
#include "entities.h"

typedef struct GameState
{
    // after adding new fields update resetState()
    unsigned int currentLevel;
    bool ballLaunched;
    unsigned int points;
    bool gameOver;
} GameState;

static inline bool isGameOver(const Ball* ball)
{
    return ball->position.y + ball->radius < 0.0f;
}

static inline void resetState(GameState* state)
{
    state->currentLevel = STARTING_LEVEL;
    state->ballLaunched = false;
    state->points = 0;
    state->gameOver = false;
}
