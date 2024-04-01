#pragma once

#include <glad/glad.h>

#include <stddef.h>

#include "entities.h"

#include "defines.h"

static inline float normalizeCoordinate(float coord) { return coord / (float)COORDINATE_SPACE * 2.0f - 1.0f; }
static inline float normalizeLength(float length) { return length / (float)COORDINATE_SPACE * 2.0f; }

Block createPaddle(float startPosX, float startPosY, float width, float height);
Ball createBall(float startPosX, float startPosY, float radius, float translationX, float translationY);
Block* createBlocks(unsigned int level, size_t* blockCount);

void removeBlock(Block* blocks, size_t* blockCount, size_t index);