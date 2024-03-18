#include "board.h"

#include <glad/glad.h>

#define INCBIN_PREFIX
#include <incbin.h>

#include <stdio.h>
#include <stdlib.h>

#include "defines.h"

INCTXT(level1, "../levels/level1.txt");

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

Block createPaddle(float startPosX, float startPosY, float width, float height)
{
    Vec2 paddlePosition = { startPosX, startPosY };
    Block paddle = {
        paddlePosition,
        width,
        height,
    };

    return paddle;
}

inline static void getLineCountAndMaxLength(const char* str, size_t* lineCount, size_t* maxLineLength)
{
    *lineCount = 0;
    *maxLineLength = 0;

    size_t lineLength = 0;

    const char* ch;
    for (ch = str; *ch; ch++)
    {
        if (*ch != '\n' && *ch != '\r')
        {
            lineLength++;
        }
        else if (*ch == '\n')
        {
            if (lineLength > *maxLineLength)
                *maxLineLength = lineLength;

            lineLength = 0;
            (*lineCount)++;
        }
    }

    if (*(ch - 1) != '\n')
        (*lineCount)++;
}

typedef enum EntityChar
{
    BLOCK_CHAR = '#',
    EMPTY_CHAR = '.'
} EntityChar;

Block* createBlocks(unsigned int level, size_t* blockCount)
{
    const char* levelData;

    switch (level)
    {
    case 1:
        levelData = level1Data;
        break;
    default:
        fprintf(stderr, "Error: Tried to load level %u, which doesn't exist!", level);
        break;
    }

    size_t maxLineLength;
    size_t lineCount;
    getLineCountAndMaxLength(levelData, &lineCount, &maxLineLength);

    float gridCellHeight = (float)COORDINATE_SPACE / (float)lineCount;
    float gridCellWidth = (float)COORDINATE_SPACE / (float)maxLineLength;
    float blockWidth = gridCellWidth - BLOCK_HORIZONTAL_PADDING * 2;
    float blockHeight = gridCellHeight - BLOCK_VERTICAL_PADDING * 2;

    *blockCount = 0;
    size_t reservedBlocksCount = 10;
    Block* blocks = malloc(sizeof(Block) * reservedBlocksCount);

    size_t row = 0;
    size_t col = 0;

    for (const char* currChar = levelData; *currChar; currChar++)
    {
        if (*currChar == BLOCK_CHAR)
        {
            if (reservedBlocksCount <= *blockCount)
            {
                reservedBlocksCount *= 2;
                blocks = realloc(blocks, sizeof(Block) * reservedBlocksCount);
            }

            Vec2 position = {
                (float)col * gridCellWidth + BLOCK_HORIZONTAL_PADDING,
                (float)(lineCount - row) * gridCellHeight - BLOCK_VERTICAL_PADDING,
            };

            blocks[*blockCount].width = blockWidth;
            blocks[*blockCount].height = blockHeight;
            blocks[*blockCount].position = position;

            (*blockCount)++;
        }

        if (*currChar == '\n')
        {
            row++;
            col = 0;
        }
        else
        {
            col++;
        }
    }

    return blocks;
}