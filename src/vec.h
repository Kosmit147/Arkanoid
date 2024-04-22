#pragma once

#include <math.h>

typedef struct Vec2
{
    float x, y;
} Vec2;

static inline Vec2 subVecs(Vec2 a, Vec2 b)
{
    Vec2 result = {
        .x = a.x - b.x,
        .y = a.y - b.y,
    };

    return result;
}

static inline Vec2 addVecs(Vec2 a, Vec2 b)
{
    Vec2 result = {
        .x = a.x + b.x,
        .y = a.y + b.y,
    };

    return result;
}

static inline float dot(Vec2 a, Vec2 b)
{
    return a.x * b.x + a.y * b.y;
}

static inline float vecLength(Vec2 vec)
{
    return sqrtf(dot(vec, vec));
}

static inline Vec2 scalar(Vec2 vec, float val)
{
    Vec2 result = {
        .x = vec.x * val,
        .y = vec.y * val,
    };

    return result;
}

static inline Vec2 scalarDiv(Vec2 vec, float val)
{
    Vec2 result = {
        .x = vec.x / val,
        .y = vec.y / val,
    };

    return result;
}

static inline Vec2 negate(Vec2 vec)
{
    Vec2 result = {
        .x = -vec.x,
        .y = -vec.y,
    };

    return result;
}

static inline Vec2 reflect(Vec2 vec, Vec2 normal)
{
    Vec2 tmp = scalar(normal, 2.0f * dot(vec, normal));
    Vec2 result = subVecs(vec, tmp);

    return result;
}

static inline Vec2 normalize(Vec2 vec)
{
    float length = vecLength(vec);
    Vec2 result = scalarDiv(vec, length);

    return result;
}
