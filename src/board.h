#pragma once

#include <glad/glad.h>

#include "entities.h"

void normalizeBlockCoordinates(float* normalizedPositions, Block* block);
Block* createBlocks(unsigned int level, size_t* blockCount);