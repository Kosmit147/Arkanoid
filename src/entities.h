#pragma once

#include "helpers.h"
#include "vec.h"

#include "defines.h"

typedef struct Block
{
    Vec2 position; // top-left corner
    // TODO: we dont need to store width and height for every block since all blocks are the same
    float width;
    float height;
} Block;

typedef struct Ball
{
    Vec2 position; // ball center
    float radius;
    Vec2 direction;
    float speed;
} Ball;

typedef struct Rect
{
    Vec2 position;
    float width;
    float height;
} Rect;

typedef struct RectBounds
{
    Vec2 topLeft;
    Vec2 bottomRight;
} RectBounds;

static inline RectBounds getBlockBorderRect(const Block* block)
{
    return (RectBounds) {
        .topLeft = (Vec2) {
            .x = block->position.x + BLOCK_BORDER_WIDTH,
            .y = block->position.y - BLOCK_BORDER_WIDTH,
        },
        .bottomRight = (Vec2) {
            .x = block->position.x + block->width - BLOCK_BORDER_WIDTH,
            .y = block->position.y - block->height + BLOCK_BORDER_WIDTH,
        },
    };
}

static inline Vec4 getRandomBlockColor()
{
    Vec4 randomColor = {
        .r = randomNormalizedFloat(),
        .g = randomNormalizedFloat(),
        .b = randomNormalizedFloat(),
        .a = 1.0f,
    };

    return saturateColor(randomColor);
}

static inline Vec4 getPaddleColor()
{
    return PADDLE_COLOR;
}

static inline Vec4 getBallColor()
{
    return BALL_COLOR;
}

static inline Vec2 getClosestPointOnBlock(const Ball* ball, const Block* block)
{
    return (Vec2) {
        .x = clamp(block->position.x, block->position.x + block->width, ball->position.x),
        .y = clamp(block->position.y - block->height, block->position.y, ball->position.y),
    };
}

static inline void reflectBall(Ball* ball, Vec2 normal)
{
    ball->direction = normalize(reflect(ball->direction, normal));
}
