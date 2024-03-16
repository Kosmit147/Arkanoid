#pragma once
#include "defines.h"
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
    Vec2 translation;
} Ball;
