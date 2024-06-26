#pragma once

#include <glad/glad.h>

typedef struct TextureOptions
{
    GLint horizontalWrap;
    GLint verticalWrap;
    GLint minFilter;
    GLint magFilter;
    const float* borderColor; // set to NULL if not using GL_CLAMP_TO_BORDER wrapping mode
    const GLint* swizzleMask; // 4 values corresponding to RGBA channels
} TextureOptions;

GLuint createTexture(GLenum slot, GLsizei width, GLsizei height, void* data, GLenum dataType,
    GLenum dataFormat, GLint internalFormat, const TextureOptions* options);

void bindTexture(GLuint texture, GLenum slot);

void freeTexture(GLuint texture);
