#pragma once

#include <glad/glad.h>

#include <stddef.h>

#include "entities.h"

#include "defines.h"

typedef struct RenderingData RenderingData;

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

GameObjects createGameObjects();

void moveGameObjects(GameObjects* objects);
void collideGameObjects(GameObjects* objects, RenderingData* renderingData);

void freeGameObjects(const GameObjects* objects);
