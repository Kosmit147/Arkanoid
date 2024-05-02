#pragma once

#include <glad/glad.h>

#include <stddef.h>
#include <stdbool.h>

#include "game_state.h"
#include "entities.h"

#include "defines.h"

typedef struct GameRenderData GameRenderData;

typedef struct GameObjects
{
    Block paddle;
    size_t blockCount;
    Block* blocks;
    Ball ball;
} GameObjects;

typedef enum Axis
{
    AXIS_VERTICAL,
    AXIS_HORIZONTAL,
} Axis;

static inline float normalizeCoordinate(float coord) { return coord / (float)COORDINATE_SPACE * 2.0f - 1.0f; }
static inline float normalizeLength(float length) { return length / (float)COORDINATE_SPACE * 2.0f; }

GameObjects createGameObjects(unsigned int level);

void moveGameObjects(GameObjects* objects);
void collideGameObjects(GameState* state, GameObjects* objects, GameRenderData* renderData);

static inline bool ballOutOfBounds(const Ball* ball) { return ball->position.y + ball->radius < 0.0f; }
static inline bool boardCleared(const GameObjects* objects) { return objects->blockCount == 0; }

void freeGameObjects(const GameObjects* objects);
