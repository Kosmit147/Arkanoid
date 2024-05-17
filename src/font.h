#pragma once

#include "texture.h"

typedef struct BitmapFont
{
    unsigned int cols;
    unsigned int rows;
    char offset; // ASCII code of the first character in the texture
    unsigned int textureID; // BitmapFont owns the texture
} BitmapFont;

static inline void freeBitmapFont(const BitmapFont* font) { freeTexture(font->textureID); }
