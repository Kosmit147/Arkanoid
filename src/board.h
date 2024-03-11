#pragma once

#include <glad/glad.h>

#include "vec.h"
#include "entities.h"

float normalizeCoordinatef(float coord);
float normalizeCoordinateui(unsigned int coord);
void normalizeBlockCoordinates(float* normalizedPositions, Vec2ui position, unsigned int width, unsigned int height);
Block* createBlock(Vec2ui position, unsigned int width, unsigned int height, GLenum usage);