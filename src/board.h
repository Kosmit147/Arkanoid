#pragma once

#include <glad/glad.h>

#include <stddef.h>

#include "entities.h"

void normalizeBlockCoordinates(float* normalizedPositions, Block* block);
Block createPaddle(float startPosX, float startPosY, float width, float height);
Block* createBlocks(unsigned int level, size_t* blockCount);