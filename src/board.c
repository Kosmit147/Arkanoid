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

Block* createBlocks(unsigned int /*level*/, size_t* blockCount)
{
    // TODO: load level from a file based on level arg

    *blockCount = 3;
    Block* blocks = malloc(sizeof(Block) * *blockCount);

    Vec2 position1 = { 0.0f, 4096.0f };
    float width1 = 1024.0f;
    float height1 = 1024.0f;

    Vec2 position2 = { 1856.0f, 3234.0f };
    float width2 = 1000.0f;
    float height2 = 200.0f;

    Vec2 position3 = { 4000.0f, 3000.0f };
    float width3 = 100.0f;
    float height3 = 150.0f;

    Block block1 = {
        position1,
        width1,
        height1,
    };

    Block block2 = {
        position2,
        width2,
        height2,
    };

    Block block3 = {
        position3,
        width3,
        height3,
    };

    blocks[0] = block1;
    blocks[1] = block2;
    blocks[2] = block3;

    return blocks;
}