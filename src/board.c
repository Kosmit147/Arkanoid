#include "board.h"

#include <glad/glad.h>

#define INCBIN_PREFIX
#include <incbin.h>

#include <stdlib.h>
#include <string.h>

#include "log.h"

INCTXT(level0, "../levels/level0.txt");
INCTXT(level1, "../levels/level1.txt");

extern float deltaTime;

Block createPaddle(float startPosX, float startPosY, float width, float height)
{
    Vec2 paddlePosition = { startPosX, startPosY };
    Block paddle = {
        .position = paddlePosition,
        .width = width,
        .height = height,
    };

    return paddle;
}

Ball createBall(float startPosX, float startPosY, float radius, float translationX, float translationY)
{
    Vec2 ballPosition = { startPosX, startPosY };
    Vec2 ballTranslation = { translationX, translationY };
    Ball ball = {
        .position = ballPosition,
        .translation = ballTranslation,
        .radius = radius,
    };

    return ball;
}

static void getLineCountAndMaxLength(const char* str, size_t* lineCount, size_t* maxLineLength)
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

    // in case there was no new line at the end of the file
    if (ch != str && *(ch - 1) != '\n')
        (*lineCount)++;
}

Block* createBlocks(unsigned int level, size_t* blockCount)
{
    const char* levelData;

    switch (level)
    {
    case 0:
        levelData = level0Data;
        break;
    case 1:
        levelData = level1Data;
        break;
    default:
        logError("Error: Tried to load level %u, which doesn't exist!", level);
        *blockCount = 0;
        return NULL;
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
    size_t reservedBlocksCount = 30;
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

GameObjects createGameObjects()
{
    GameObjects gameObjects;

    gameObjects.paddle = createPaddle(PADDLE_START_POS_X, PADDLE_START_POS_Y, PADDLE_WIDTH, PADDLE_HEIGHT);
    gameObjects.blocks = createBlocks(STARTING_LEVEL, &gameObjects.blockCount);
    gameObjects.ball = createBall(BALL_START_POS_X, BALL_START_POS_Y, BALL_RADIUS, 0.0f, 0.0f);

    return gameObjects;
}

void freeGameObjects(const GameObjects* objects)
{
    free(objects->blocks);
}

void moveBall(Ball* ball)
{
    ball->position.x += ball->translation.x * deltaTime;
    ball->position.y += ball->translation.y * deltaTime;
}

void moveGameObjects(GameObjects* objects)
{
    moveBall(&objects->ball);
}

void removeBlock(Block* blocks, size_t* blockCount, size_t index)
{
    size_t blocksToMove = *blockCount - index - 1;
    Block* dst = &blocks[index];
    Block* src = &blocks[index + 1];
    size_t dataSize = sizeof(Block) * blocksToMove;

    memmove(dst, src, dataSize);

    (*blockCount)--;
}