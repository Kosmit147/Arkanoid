#include "board.h"

#include <glad/glad.h>

#define INCBIN_PREFIX
#include <incbin.h>

#include <stdbool.h>
#include <assert.h>

#include "game_time.h"
#include "helpers.h"
#include "memory.h"
#include "rendering.h"
#include "game_state.h"
#include "entities.h"

#include "defines.h"

#ifdef _DEBUG
INCTXT(level0, "../levels/level0.txt"); // debug level
#endif

INCTXT(level1, "../levels/level1.txt");

Vec2 normalizePoint(Vec2 point)
{
    return (Vec2) {
        .x = normalizeCoordinate(point.x),
        .y = normalizeCoordinate(point.y),
    };
}

Rect normalizeRect(Rect rect)
{
    return (Rect) {
        .position = {
            .x = normalizeCoordinate(rect.position.x),
            .y = normalizeCoordinate(rect.position.y),
        },
        .width = normalizeLength(rect.width),
        .height = normalizeLength(rect.height),
    };
}

RectBounds normalizeRectBounds(RectBounds rect)
{
    return (RectBounds) {
        .topLeft = {
            .x = normalizeCoordinate(rect.topLeft.x),
            .y = normalizeCoordinate(rect.topLeft.y),
        },
        .bottomRight = {
            .x = normalizeCoordinate(rect.bottomRight.x),
            .y = normalizeCoordinate(rect.bottomRight.y),
        },
    };
}

static Block createPaddle(Vec2 position, float width, float height)
{
    return (Block) {
        .position = position,
        .width = width,
        .height = height,
    };
}

static Ball createBall(Vec2 position, float radius, Vec2 direction, float speed)
{
    return (Ball) {
        .position = position,
        .radius = radius,
        .direction = direction,
        .speed = speed,
    };
}

typedef struct LevelData
{
    size_t gridColCount;
    size_t gridRowCount;
    size_t blockCount;
} LevelData;

static LevelData getLevelData(const char* levelStr)
{
    LevelData levelData = {
        .gridColCount = 0,
        .gridRowCount = 0,
        .blockCount = 0,
    };

    size_t currLineLength = 0;

    const char* ch;

    for (ch = levelStr; *ch; ch++)
    {
        if (*ch == BLOCK_CHAR)
        {
            levelData.blockCount++;
            currLineLength++;
        }
        else if (*ch != '\n' && *ch != '\r')
        {
            currLineLength++;
        }
        else if (*ch == '\n')
        {
            if (currLineLength > levelData.gridColCount)
                levelData.gridColCount = currLineLength;

            currLineLength = 0;
            levelData.gridRowCount++;
        }
    }

    // in case there was no new line at the end of the file
    if (ch != levelStr && *(ch - 1) != '\n')
        levelData.gridRowCount++;

    return levelData;
}

static const char* getLevelStr(unsigned int level)
{
    static_assert(STARTING_LEVEL == 0 || STARTING_LEVEL == 1,
        "Expected STARTING_LEVEL == 0 || STARTING_LEVEL == 1");

    switch (level)
    {
#ifdef _DEBUG
    case 0:
        return level0Data;
#else
    case 0:
        return NULL;
#endif
    case 1:
        return level1Data;
    default:
        // TODO: generate a level randomly
        return getLevelStr(STARTING_LEVEL);
    }
}

static Block* createBlocks(unsigned int level, size_t* blockCount)
{
    const char* levelStr = getLevelStr(level);
    LevelData levelData = getLevelData(levelStr);

    *blockCount = 0;
    Block* blocks = checkedMalloc(sizeof(Block) * levelData.blockCount);

    float gridCellHeight = (float)COORDINATE_SPACE / (float)levelData.gridRowCount;
    float gridCellWidth = (float)COORDINATE_SPACE / (float)levelData.gridColCount;
    float blockWidth = gridCellWidth - BLOCK_HORIZONTAL_PADDING * 2.0f;
    float blockHeight = gridCellHeight - BLOCK_VERTICAL_PADDING * 2.0f;

    size_t row = 0;
    size_t col = 0;

    for (const char* currChar = levelStr; *currChar; currChar++)
    {
        if (*currChar == BLOCK_CHAR)
        {
            blocks[(*blockCount)++] = (Block) {
                .position = {
                    .x = (float)col * gridCellWidth + BLOCK_HORIZONTAL_PADDING,
                    .y = (float)(levelData.gridRowCount - row) * gridCellHeight - BLOCK_VERTICAL_PADDING,
                },
                .width = blockWidth,
                .height = blockHeight,
            };
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

static QuadTree createBlocksQuadTree(const Block* blocks, size_t blockCount)
{
    QuadTree tree = quadTreeCreate((RectBounds) {
        .topLeft = { .x = 0.0f, .y = (float)COORDINATE_SPACE },
        .bottomRight = { .x = (float)COORDINATE_SPACE, .y = 0.0f, }
    });

    for (size_t i = 0; i < blockCount; i++)
        quadTreeInsert(&tree, &blocks[i]);

    return tree;
}

void initBoard(Board* board, unsigned int level)
{
    board->paddle = createPaddle((Vec2){ .x = PADDLE_START_POS_X, .y = PADDLE_START_POS_Y }, PADDLE_WIDTH,
        PADDLE_HEIGHT);
    board->blocksStorage = createBlocks(level, &board->initialBlockCount);
    board->blocksQuadTree = createBlocksQuadTree(board->blocksStorage, board->initialBlockCount);
    board->ball = createBall((Vec2){ .x = BALL_START_POS_X, .y = BALL_START_POS_Y }, BALL_RADIUS,
        (Vec2){ .x = BALL_LAUNCH_DIRECTION_X, .y = BALL_LAUNCH_DIRECTION_Y }, 0.0f);
    board->tmpRetrievedBlocksStorage = vectorCreate();
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

    // TODO: position correction
}

// returns true if there was a collision
static bool collideBallWithBlock(Ball* ball, const Block* block)
{
    Vec2 collisionPoint = getClosestPointOnBlock(ball, block);
    Vec2 difference = subVecs(ball->position, collisionPoint);
    float distSquared = dot(difference, difference);

    if (distSquared < powf(ball->radius, 2.0f))
    {
        Vec2 normal = normalize(difference);
        reflectBall(ball, normal);
        return true;
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
        float angle = getPaddleBounceAngle(paddle, collisionPoint);
        ball->direction = vecFromAngle(angle);
    }
}

void collideBall(GameState* state, Board* board, Renderer* renderer)
{
    vectorClear(&board->tmpRetrievedBlocksStorage);
    quadTreeRetrieveAllByBounds(&board->blocksQuadTree, getBallRectBounds(&board->ball),
        &board->tmpRetrievedBlocksStorage);
    size_t retrievedCount = vectorSize(&board->tmpRetrievedBlocksStorage, sizeof(const Block*));

    collideBallWithWalls(&board->ball);
    collideBallWithPaddle(&board->ball, &board->paddle);

    for (size_t i = 0; i < retrievedCount; i++)
    {
        const Block* blockPtr = *(const Block**)vectorGet(&board->tmpRetrievedBlocksStorage, i,
            sizeof(const Block*));

        if (collideBallWithBlock(&board->ball, blockPtr))
        {
            size_t blockIndex = (size_t)(blockPtr - board->blocksStorage);
            quadTreeRemoveBlock(&board->blocksQuadTree, blockPtr);
            moveBlockOutOfView(&renderer->gameRenderer, blockIndex);

            state->boardCleared = board->blocksQuadTree.elemCount == 0;

            state->points += POINTS_PER_BLOCK_DESTROYED;
            updateHudPointsText(&renderer->hudRenderer, state->points);
        }
    }
}

void freeBoard(Board* board)
{
    quadTreeFree(&board->blocksQuadTree);
    free(board->blocksStorage);
    vectorFree(&board->tmpRetrievedBlocksStorage);
}
