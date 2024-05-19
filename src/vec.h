#pragma once

#include <math.h>

#include "helpers.h"

typedef struct Vec2
{
    union { float x; float r; };
    union { float y; float g; };
} Vec2;

typedef struct Vec3
{
    union { float x; float r; };
    union { float y; float g; };
    union { float z; float b; };
} Vec3;

typedef struct Vec4
{
    union { float x; float r; };
    union { float y; float g; };
    union { float z; float b; };
    union { float w; float a; };
} Vec4;

static inline Vec2 subVecs(Vec2 a, Vec2 b)
{
    return (Vec2){ .x = a.x - b.x, .y = a.y - b.y };
}

static inline Vec2 addVecs(Vec2 a, Vec2 b)
{
    return (Vec2){ .x = a.x + b.x, .y = a.y + b.y };
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
    return (Vec2){ .x = vec.x * val, .y = vec.y * val };
}

static inline Vec2 scalarDiv(Vec2 vec, float val)
{
    return (Vec2){ .x = vec.x / val, .y = vec.y / val };
}

static inline Vec2 negate(Vec2 vec)
{
    return (Vec2){ .x = -vec.x, .y = -vec.y };
}

static inline Vec2 reflect(Vec2 vec, Vec2 normal)
{
    Vec2 tmp = scalar(normal, 2.0f * dot(vec, normal));
    return subVecs(vec, tmp);
}

static inline Vec2 normalize(Vec2 vec)
{
    return scalarDiv(vec, vecLength(vec));
}

static inline Vec2 vecFromAngle(float angle)
{
    return (Vec2) { .x = cosf(angle), .y = sinf(angle) };
}

static inline Vec4 saturateColor(Vec4 color)
{
    float max = max(max(color.r, color.g), color.b);

    if (max == 0.0f)
        return color;

    float multiplier = 1.0f / max;

    return (Vec4) {
        .r = color.r * multiplier,
        .g = color.g * multiplier,
        .b = color.b * multiplier,
        .a = color.a,
    };
}
