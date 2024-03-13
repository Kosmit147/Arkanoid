#pragma once

#include <glad/glad.h>

#include "vec.h"
#include "entities.h"

void normalizeBlockCoordinates(float* normalizedPositions, Vec2 position, unsigned int width, unsigned int height);
Block* createBlock(Vec2 position, unsigned int width, unsigned int height, GLenum usage);