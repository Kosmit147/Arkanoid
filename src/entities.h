#pragma once

#include <stdbool.h>

#include "helpers.h"
#include "vec.h"

typedef struct Block
{
    Vec2 position; // top-left corner
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

static inline Vec4 getRandomPaddleColor()
{
    return getRandomBlockColor();
}

static inline Vec2 getClosestPointOnBlock(const Ball* ball, const Block* block)
{
    return (Vec2)
    {
        .x = clamp(block->position.x, block->position.x + block->width, ball->position.x),
            .y = clamp(block->position.y - block->height, block->position.y, ball->position.y),
    };
}

static inline void reflectBall(Ball* ball, Vec2 normal)
{
    ball->direction = normalize(reflect(ball->direction, normal));
}
