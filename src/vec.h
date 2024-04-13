#pragma once
#include <math.h>
typedef struct Vec2
{
    float x, y;
} Vec2;

static inline Vec2 subtractVec2(Vec2 a, Vec2 b)
{
    Vec2 result = {
        .x = a.x - b.x,
        .y = a.y - b.y,
    };

    return result;
}

static inline float lengthOfVec(Vec2 vec)
{
    return (float)sqrt(vec.x * vec.x + vec.y * vec.y);
}

static inline float dot(Vec2 a, Vec2 b)
{
    return a.x * b.x + a.y * b.y;
}