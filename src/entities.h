#pragma once

#include "vec.h"

typedef struct Ball
{
    Vec2 position; // ball center
    Vec2 translation;

    unsigned int glVB; // vertex buffer
} Ball;

typedef struct Block
{
    Vec2 position; // top-left corner
    unsigned int width;
    unsigned int height;

    unsigned int glVB; // vertex buffer
} Block;