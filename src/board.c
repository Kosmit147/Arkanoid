#include "board.h"

#include <glad/glad.h>

#include <stdlib.h>

#include "rendering.h"

#include "defines.h"

float normalizeCoordinate(float coord)
{
    return coord / (float)COORDINATE_SPACE * 2.0f - 1.0f;
}

void normalizeBlockCoordinates(float* normalizedPositions, Block* block)
{
    float normalizedX1 = normalizeCoordinate(block->position.x);
    float normalizedY1 = normalizeCoordinate(block->position.y);
    float normalizedX2 = normalizedX1 + block->width / COORDINATE_SPACE * 2.0f;
    float normalizedY2 = normalizedY1 - block->height / COORDINATE_SPACE * 2.0f;

    normalizedPositions[0] = normalizedX1; normalizedPositions[1] = normalizedY1;
    normalizedPositions[2] = normalizedX2; normalizedPositions[3] = normalizedY1;
    normalizedPositions[4] = normalizedX2; normalizedPositions[5] = normalizedY2;
    normalizedPositions[6] = normalizedX1; normalizedPositions[7] = normalizedY2;
}