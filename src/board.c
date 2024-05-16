#include "board.h"

#include <glad/glad.h>

#define INCBIN_PREFIX
#include <incbin.h>

#include <stdbool.h>
#include <assert.h>

#include "game_time.h"
#include "log.h"
#include "helpers.h"
#include "vector.h"
#include "rendering.h"
#include "game_state.h"
#include "entities.h"
#include "structures.h"
#include "defines.h"

INCTXT(level0, "../levels/level0.txt");
INCTXT(level1, "../levels/level1.txt");

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
    static_assert(STARTING_LEVEL == 0 || STARTING_LEVEL == 1, "Expected STARTING_LEVEL == 0 || STARTING_LEVEL == 1");

    switch (level)
    {
    case 0:
        return level0Data;
    case 1:
        return level1Data;
    default:
        // TODO: generate a level randomly
        return getLevelData(STARTING_LEVEL);
    }
}

static Block* createBlocks(unsigned int level, size_t* blockCount)
{
    Block bounds = { {.x = 0,.y = 0}, COORDINATE_SPACE, COORDINATE_SPACE / 2 };
    globalQuadTree = createQuadtree(0, bounds);
    *blockCount = 0;

    const char* levelData = getLevelData(level);

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


            insert(globalQuadTree, &newBlock);
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
    display(globalQuadTree);
    return blocksVector.data;
}

GameObjects createGameObjects(unsigned int level)
{
    GameObjects gameObjects;

    gameObjects.paddle = createPaddle((Vec2) { .x = PADDLE_START_POS_X, .y = PADDLE_START_POS_Y },
        PADDLE_WIDTH, PADDLE_HEIGHT);
    gameObjects.blocks = createBlocks(level, &gameObjects.blockCount);
    gameObjects.ball = createBall((Vec2) { .x = BALL_START_POS_X, .y = BALL_START_POS_Y }, BALL_RADIUS,
        (Vec2)
    {
        .x = BALL_LAUNCH_DIRECTION_X, .y = BALL_LAUNCH_DIRECTION_Y
    }, 0.0f);

    return gameObjects;
}

void moveBall(Ball* ball)
{
    ball->position.x += ball->direction.x * ball->speed * subStepDeltaTime;
    ball->position.y += ball->direction.y * ball->speed * subStepDeltaTime;
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
    Vec2 collisionPoint = getClosestPointOnBlock(ball, block);
    Vec2 difference = subVecs(ball->position, collisionPoint);
    float distSquared = dot(difference, difference);

    if (distSquared < powf(ball->radius, 2.0f))
    {
        // fail-safe in case both x and y are 0.0 (in that case we can't normalize)
        // TODO: remove once collisions work properly
        if (difference.x != 0.0f || difference.y != 0.0f)
        {
            Vec2 normal = normalize(difference);
            reflectBall(ball, normal);
            return true;
        }
    }

    return false;
}

static float getPaddleBounceAngle(const Block* paddle, Vec2 collisionPoint)
{
    // divide the angle based on where the collision happened
    // for example divide by half if the ball collided with the paddle on a point
    // 3/4 of the way through from the starting to the ending edge of the paddle
    // (because it's 1/2 of the way through from the middle of the paddle to the end)
    float multiplier = (paddle->position.x + paddle->width - collisionPoint.x) / paddle->width;
    float minAngle = (float)MIN_BALL_BOUNCE_ANGLE_OFF_PADDLE;
    float maxAngle = (float)MATH_PI - (float)MIN_BALL_BOUNCE_ANGLE_OFF_PADDLE;
    float angle = (float)MATH_PI * multiplier;
    return clamp(minAngle, maxAngle, angle);
}

static void collideBallWithPaddle(Ball* ball, const Block* paddle)
{
    Vec2 collisionPoint = getClosestPointOnBlock(ball, paddle);
    Vec2 difference = subVecs(ball->position, collisionPoint);
    float distSquared = dot(difference, difference);

    if (distSquared < powf(ball->radius, 2.0f))
    {
        // fail-safe in case both x and y are 0.0 (in that case we can't normalize)
        // TODO: remove once collisions work properly
        if (difference.x != 0.0f || difference.y != 0.0f)
        {
            float angle = getPaddleBounceAngle(paddle, collisionPoint);
            ball->direction = vecFromAngle(angle);
        }
    }
}

static void removeBlockAndUpdateInstanceBuffer(Block* blocks, size_t blockCount, size_t removedIndex, unsigned int instanceBuffer)
{
    eraseFromArr(blocks, removedIndex, blockCount, sizeof(Block));
    eraseObjectFromGLBuffer(GL_ARRAY_BUFFER, instanceBuffer, removedIndex, blockCount, BLOCK_INSTANCE_VERTICES_SIZE);
}


void collideBall(GameState* state, GameObjects* gameObjects, GameRenderData* renderData)
{

    Block** retrievedBlocks = (Block**)malloc(MAX_OBJECTS * sizeof(Block*));
    size_t count = 0;
    retrieveBlocks(globalQuadTree, gameObjects->ball.position, retrievedBlocks, &count);

    collideBallWithWalls(&gameObjects->ball);
    collideBallWithPaddle(&gameObjects->ball, &gameObjects->paddle);



    for (size_t i = 0; i < count; i++)
    {
        if (collideBallWithBlock(&gameObjects->ball, retrievedBlocks[i]))
        {
            removeBlock(globalQuadTree, retrievedBlocks[i]);
            removeBlockAndUpdateInstanceBuffer(gameObjects->blocks, gameObjects->blockCount--,
                i--, renderData->blocksRenderer.instanceBuffer);

            state->points += POINTS_PER_BLOCK_DESTROYED;
            logNotification("Points: %u\n", state->points); // TODO: change once text rendering works
        }
    }
}

void freeGameObjects(const GameObjects* objects)
{
    free(objects->blocks);
}
