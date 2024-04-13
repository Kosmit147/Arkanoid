#include "board.h"

#include <glad/glad.h>

#define INCBIN_PREFIX
#include <incbin.h>

#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "helpers.h"

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

Ball createBall(float startPosX, float startPosY, float radius, float directionX, float directionY, float speed)
{
    Vec2 ballPosition = { startPosX, startPosY };
    Vec2 ballDirection = { directionX,  directionY };
    Ball ball = {
        .position = ballPosition,
        .direction = ballDirection,
        .radius = radius,
        .speed = speed,
    };

    return ball;
}

static void getLineCountAndMaxLineLength(const char* str, size_t* lineCount, size_t* maxLineLength)
{
    *lineCount = 0;
    *maxLineLength = 0;

    size_t currLineLength = 0;

    const char* ch;
    for (ch = str; *ch; ch++)
    {
        if (*ch != '\n' && *ch != '\r')
        {
            currLineLength++;
        }
        else if (*ch == '\n')
        {
            if (currLineLength > *maxLineLength)
                *maxLineLength = currLineLength;

            currLineLength = 0;
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
    getLineCountAndMaxLineLength(levelData, &lineCount, &maxLineLength);

    float gridCellHeight = (float)COORDINATE_SPACE / (float)lineCount;
    float gridCellWidth = (float)COORDINATE_SPACE / (float)maxLineLength;
    float blockWidth = gridCellWidth - BLOCK_HORIZONTAL_PADDING * 2.0f;
    float blockHeight = gridCellHeight - BLOCK_VERTICAL_PADDING * 2.0f;

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
    gameObjects.ball = createBall(BALL_START_POS_X, BALL_START_POS_Y, BALL_RADIUS, BALL_LAUNCH_DIRECTION_X, BALL_LAUNCH_DIRECTION_Y, 0.0f);

    return gameObjects;
}

void freeGameObjects(const GameObjects* objects)
{
    free(objects->blocks);
}

typedef enum Direction
{
    VERTICAL, HORIZONTAL
} Direction;

static void moveBall(Ball* ball)
{
    ball->position.y += ball->direction.y * deltaTime * ball->speed;
    ball->position.x += ball->direction.x * deltaTime * ball->speed;
}

void moveGameObjects(GameObjects* objects)
{
    moveBall(&objects->ball);
}

void flipBallDirection(Direction direction, Ball* ball)
{
    switch (direction)
    {
    case VERTICAL:
        ball->direction.y = -ball->direction.y;
        break;
    case HORIZONTAL:
        ball->direction.x = -ball->direction.x;
        break;
    }
}

static void collideBallWithWalls(Ball* ball)
{
    if (ball->position.y + ball->radius >= COORDINATE_SPACE)
        flipBallDirection(VERTICAL, ball);
    else if (ball->position.x - ball->radius < 0.0f)
        flipBallDirection(HORIZONTAL, ball);
    else if (ball->position.x + ball->radius > COORDINATE_SPACE)
        flipBallDirection(HORIZONTAL, ball);
}

static void collideBallWithPaddle(Ball* /*ball*/, const Block* /*paddle*/)
{
    //todo
}

static void collideBallWithBlock(Ball* ball, Block* block, size_t blockCount, size_t blockIndex, Block* blocks)
{
    Vec2 closestPoint =
    {
        .x = clamp(block->position.x, block->position.x + block->width, ball->position.x),
        .y = clamp(block->position.y - block->height, block->position.y, ball->position.y),
    };

    Vec2 difference = subtractVec2(ball->position, closestPoint);

    float distanceSquared = dot(difference, difference);

    if (distanceSquared < ball->radius * ball->radius)
    {
        if (withinRange(block->position.x, block->position.x + block->width, ball->position.x))
        {
            flipBallDirection(VERTICAL, ball);
        }
        else
        {
            flipBallDirection(HORIZONTAL, ball);
        }
        removeBlock(blocks, &blockCount, blockIndex);
    }

}

static void collideBall(Ball* ball, Block* paddle, Block* blocks, size_t blockCount)
{
    collideBallWithWalls(ball);
    collideBallWithPaddle(ball, paddle);

    for (size_t i = 0; i < blockCount; i++)
    {
        collideBallWithBlock(ball, &blocks[i], blockCount, i, blocks);
    }

}

void collideGameObjects(GameObjects* objects)
{
    collideBall(&objects->ball, &objects->paddle, objects->blocks, objects->blockCount);
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