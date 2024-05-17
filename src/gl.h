#pragma once

#include <glad/glad.h>

#include <stddef.h>

#include "vec.h"
#include "font.h"

typedef struct QuadRenderer
{
    unsigned int VA;
    unsigned int VB;
} QuadRenderer;

typedef struct InstancedQuadRenderer
{
    unsigned int VA;
    unsigned int VB;
    unsigned int instanceBuffer;
} InstancedQuadRenderer;

typedef struct TextRenderer
{
    unsigned int VA;
    unsigned int VB;
    size_t charCount;
    float charWidth;
    float charHeight;
    const BitmapFont* font;
} TextRenderer;

#ifdef _DEBUG
void rendererGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
    const GLchar* message, const void* userParam);
#endif

unsigned int genVA();
unsigned int genVB();
unsigned int genIB();

TextRenderer createTextRenderer(const char* text, size_t textLength, const BitmapFont* font, Vec2 position,
    float charWidth, float charHeight, unsigned int quadIB);
void updateTextRenderer(TextRenderer* renderer, const char* newText, size_t newTextLength, Vec2 newPosition);

int retrieveUniformLocation(unsigned int shader, const char* name);

void drawElements(unsigned int VA, GLsizei count, GLenum IBType);
void drawInstances(unsigned int VA, GLsizei vertexCount, GLsizei instanceCount, GLenum IBType);
void renderText(const TextRenderer* renderer);

void moveDataWithinGLBuffer(GLenum bufferType, unsigned int buffer, GLintptr dstOffset, GLintptr srcOffset,
    GLsizeiptr size);
void moveObjectsWithinGLBuffer(GLenum bufferType, unsigned int buffer, size_t dstIndex, size_t srcIndex,
    size_t count, size_t objSize);
void eraseObjectFromGLBuffer(GLenum bufferType, unsigned int buffer, size_t index, size_t objectCount,
    size_t objSize);

unsigned int createQuadIB(size_t count, GLenum usage);

void freeQuadRenderer(const QuadRenderer* renderer);
void freeInstancedQuadRenderer(const InstancedQuadRenderer* renderer);
void freeTextRenderer(const TextRenderer* renderer);
