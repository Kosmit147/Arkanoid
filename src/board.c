#include "board.h"

#include <glad/glad.h>

#define INCBIN_PREFIX
#include <incbin.h>

#include <stdbool.h>

#include "log.h"
#include "helpers.h"
#include "defines.h"
#include "vector.h"
#include "rendering.h"

INCTXT(level0, "../levels/level0.txt");
INCTXT(level1, "../levels/level1.txt");

extern float subStepDeltaTime;

static Block createPaddle(Vec2 position, float width, float height)
{
    return (Block)
    {
        .position = position,
        .width = width,
        .height = height,
    };
}

static Ball createBall(Vec2 position, float radius, Vec2 direction, float speed)
{
    return (Ball)
    {
        .position = position,
        .radius = radius,
        .direction = direction,
        .speed = speed,
    };
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

static const char* getLevelData(unsigned int level)
{
    switch (level)
    {
    case 0:
        return level0Data;
    case 1:
        return level1Data;
    default:
        return NULL;
    }
}

static Block* createBlocks(unsigned int level, size_t* blockCount)
{
    *blockCount = 0;

    const char* levelData = getLevelData(level);

    if (!levelData)
    {
        logError("Error: Tried to load level %u, which doesn't exist!", level);
        return NULL;
    }

    Vector blocksVector = vectorCreate();
    vectorReserve(&blocksVector, 30, Block);

    size_t maxLineLength;
    size_t lineCount;
    getLineCountAndMaxLineLength(levelData, &lineCount, &maxLineLength);

    float gridCellHeight = (float)COORDINATE_SPACE / (float)lineCount;
    float gridCellWidth = (float)COORDINATE_SPACE / (float)maxLineLength;
    float blockWidth = gridCellWidth - BLOCK_HORIZONTAL_PADDING * 2.0f;
    float blockHeight = gridCellHeight - BLOCK_VERTICAL_PADDING * 2.0f;

    size_t row = 0;
    size_t col = 0;

    for (const char* currChar = levelData; *currChar; currChar++)
    {
        if (*currChar == BLOCK_CHAR)
        {
            Vec2 position = {
                .x = (float)col * gridCellWidth + BLOCK_HORIZONTAL_PADDING,
                .y = (float)(lineCount - row) * gridCellHeight - BLOCK_VERTICAL_PADDING,
            };

            Block newBlock = {
                .position = position,
                .width = blockWidth,
                .height = blockHeight,
            };

            vectorPushBack(&blocksVector, &newBlock, Block);
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

    return blocksVector.data;
}

GameObjects createGameObjects()
{
    GameObjects gameObjects;

    gameObjects.paddle = createPaddle((Vec2) { .x = PADDLE_START_POS_X, .y = PADDLE_START_POS_Y },
        PADDLE_WIDTH, PADDLE_HEIGHT);
    gameObjects.blocks = createBlocks(STARTING_LEVEL, &gameObjects.blockCount);
    gameObjects.ball = createBall((Vec2) { .x = BALL_START_POS_X, .y = BALL_START_POS_Y }, BALL_RADIUS,
        (Vec2)
    {
        .x = BALL_LAUNCH_DIRECTION_X, .y = BALL_LAUNCH_DIRECTION_Y
    }, 0.0f);

    return gameObjects;
}

static void moveBall(Ball* ball)
{
    ball->position.x += ball->direction.x * ball->speed * subStepDeltaTime;
    ball->position.y += ball->direction.y * ball->speed * subStepDeltaTime;
}

void moveGameObjects(GameObjects* objects)
{
    moveBall(&objects->ball);
}

static void flipBallDirectionOnAxis(Axis axis, Ball* ball)
{
    switch (axis)
    {
    case AXIS_VERTICAL:
        ball->direction.y = -ball->direction.y;
        break;
    case AXIS_HORIZONTAL:
        ball->direction.x = -ball->direction.x;
        break;
    }
}

static void collideBallWithWalls(Ball* ball)
{
    if (ball->position.y + ball->radius > COORDINATE_SPACE)
        flipBallDirectionOnAxis(AXIS_VERTICAL, ball);
    else if (ball->position.x - ball->radius < 0.0f)
        flipBallDirectionOnAxis(AXIS_HORIZONTAL, ball);
    else if (ball->position.x + ball->radius > COORDINATE_SPACE)
        flipBallDirectionOnAxis(AXIS_HORIZONTAL, ball);
}

// returns true if there was a collision
static bool collideBallWithBlock(Ball* ball, const Block* block)
{
    Vec2 closestPoint = getClosestPointOnBlock(ball, block);
    Vec2 difference = subVecs(ball->position, closestPoint);
    float distSquared = dot(difference, difference);

    if (distSquared < powf(ball->radius, 2.0f))
    {
        // fail-safe in case both x and y are 0.0 (in that case we can't normalize)
        if (difference.x != 0.0f || difference.y != 0.0f)
        {
            Vec2 normal = normalize(difference);
            reflectBall(ball, normal);
            return true;
        }
    }

    return false;
}

static void collideBallWithPaddle(Ball* ball, const Block* paddle)
{
    Vec2 closestPoint = getClosestPointOnBlock(ball, paddle);
    Vec2 difference = subVecs(ball->position, closestPoint);
    float multiplier = (paddle->position.x + paddle->width - closestPoint.x) / paddle->width;
    float distSquared = dot(difference, difference);
    if (distSquared < powf(ball->radius, 2.0f))
    {
        // fail-safe in case both x and y are 0.0 (in that case we can't normalize)
        if (difference.x != 0.0f || difference.y != 0.0f)
        {

            float angle = (float)M_PI * multiplier;
            ball->direction.x = cosf(angle);
            ball->direction.y = sinf(angle);

        }
    }

}

static void removeBlockAndUpdateInstanceBuffer(Block* blocks, size_t blockCount, size_t removedIndex, unsigned int instanceBuffer)
{
    eraseFromArr(blocks, removedIndex, blockCount, sizeof(Block));
    eraseObjectFromGLBuffer(GL_ARRAY_BUFFER, instanceBuffer, removedIndex, blockCount, BLOCK_INSTANCE_VERTICES_SIZE);
}

static void collideBall(GameObjects* gameObjects, GameRenderData* renderData)
{
    collideBallWithWalls(&gameObjects->ball);
    collideBallWithPaddle(&gameObjects->ball, &gameObjects->paddle);

    for (size_t i = 0; i < gameObjects->blockCount; i++)
    {
        if (collideBallWithBlock(&gameObjects->ball, &gameObjects->blocks[i]))
        {
            removeBlockAndUpdateInstanceBuffer(gameObjects->blocks, gameObjects->blockCount--,
                i--, renderData->blocksQuad.instanceBuffer);
        }
    }
}

void collideGameObjects(GameObjects* objects, GameRenderData* renderData)
{
    collideBall(objects, renderData);
}

void freeGameObjects(const GameObjects* objects)
{
    free(objects->blocks);
}
