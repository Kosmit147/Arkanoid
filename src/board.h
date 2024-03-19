#pragma once

#include <glad/glad.h>

#include <stddef.h>

#include "entities.h"

void normalizeBlockCoordinates(float* normalizedPositions, Block* block);
float normalizeCoordinate(float coord);
Block createPaddle(float startPosX, float startPosY, float width, float height);
Ball createBall(float startPosX, float startPosY, float radius, float translationX, float translationY);
Block* createBlocks(unsigned int level, size_t* blockCount);