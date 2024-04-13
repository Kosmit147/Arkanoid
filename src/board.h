#pragma once

#include <glad/glad.h>

#include <stddef.h>

#include "entities.h"

#include "defines.h"

typedef struct GameObjects
{
    Block paddle;
    size_t blockCount;
    Block* blocks;
    Ball ball;
} GameObjects;

static inline float normalizeCoordinate(float coord) { return coord / (float)COORDINATE_SPACE * 2.0f - 1.0f; }
static inline float normalizeLength(float length) { return length / (float)COORDINATE_SPACE * 2.0f; }

Block createPaddle(float startPosX, float startPosY, float width, float height);
Ball createBall(float startPosX, float startPosY, float radius, float directionX, float directionY, float speed);
Block* createBlocks(unsigned int level, size_t* blockCount);

GameObjects createGameObjects();

void collideGameObjects(GameObjects* objects);

void freeGameObjects(const GameObjects* objects);

void moveGameObjects(GameObjects* objects);

void removeBlock(Block* blocks, size_t* blockCount, size_t index);