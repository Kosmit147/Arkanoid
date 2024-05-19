#pragma once

#include <glad/glad.h>

#include <stddef.h>

#include "vec.h"
#include "font.h"

#define vertexAttribfv(location, type, name) { glVertexAttribPointer((location),\
    sizeof(((type*)NULL)->name) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(type),\
    (void*)offsetof(type, name));\
    glEnableVertexAttribArray((location)); }

#define instVertexAttribfv(location, type, name) { glVertexAttribPointer((location),\
    sizeof(((type*)NULL)->name) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(type),\
    (void*)offsetof(type, name));\
    glEnableVertexAttribArray((location));\
    glVertexAttribDivisor((location), 1); }

typedef struct QuadRenderer
{
    GLuint VA;
    GLuint VB;
} QuadRenderer;

typedef struct InstancedQuadRenderer
{
    GLuint VA;
    GLuint VB;
    GLuint instanceBuffer;
} InstancedQuadRenderer;

typedef struct TextRenderer
{
    GLuint VA;
    GLuint VB;
    size_t charCount;
    Vec2 position;
    float charWidth;
    float charHeight;
    const BitmapFont* font;
} TextRenderer;

#ifdef _DEBUG
void rendererGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
    const GLchar* message, const void* userParam);
#endif

GLuint genVA();
GLuint genVB();
GLuint genIB();

TextRenderer createTextRenderer(const char* text, size_t textLength, const BitmapFont* font, Vec2 position,
    float charWidth, float charHeight, GLuint quadIB);
void updateTextRenderer(TextRenderer* renderer, const char* newText, size_t newTextLength, Vec2 newPosition);

GLint retrieveUniformLocation(GLuint shader, const GLchar* name);

void drawElements(GLuint VA, GLsizei count, GLenum IBType);
void drawInstances(GLuint VA, GLsizei vertexCount, GLsizei instanceCount, GLenum IBType);
void renderText(const TextRenderer* renderer);

void moveDataWithinGLBuffer(GLenum bufferType, GLuint buffer, GLintptr dstOffset, GLintptr srcOffset,
    GLsizeiptr size);
void moveObjectsWithinGLBuffer(GLenum bufferType, GLuint buffer, size_t dstIndex, size_t srcIndex,
    size_t count, size_t objSize);
void eraseObjectFromGLBuffer(GLenum bufferType, GLuint buffer, size_t index, size_t objectCount,
    size_t objSize);

GLuint createQuadIB(size_t count, GLenum usage);

void freeQuadRenderer(const QuadRenderer* renderer);
void freeInstancedQuadRenderer(const InstancedQuadRenderer* renderer);
void freeTextRenderer(const TextRenderer* renderer);
