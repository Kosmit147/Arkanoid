#pragma once

#include <stdbool.h>
#include <stdio.h>

#include "defines.h"
#include "entities.h"

typedef struct GameState
{
    bool ballLaunched;
    unsigned int points;
    bool isGameOver;
} GameState;


static inline void isGameOver(Ball* ball, GameState* state)
{
    if (ball->position.y + ball->radius < 0)
    {
        printf("Game Over \n Points: %d", state->points);
        state->isGameOver = true;
    }
}

static inline void resetState(GameState* state)
{
    state->points = 0;
    state->isGameOver = false;
    state->ballLaunched = false;
}