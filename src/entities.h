#pragma once

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
    Vec2 direction;
    float speed;
    float radius;
} Ball;

static inline Vec2 getClosestPointOnBlock(const Ball* ball, const Block* block)
{
    Vec2 result = {
        .x = clamp(block->position.x, block->position.x + block->width, ball->position.x),
        .y = clamp(block->position.y - block->height, block->position.y, ball->position.y),
    };

    return result;
}