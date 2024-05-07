#pragma once

#include <glad/glad.h>

#include <stddef.h>

typedef struct GLQuadRenderer
{
    unsigned int VA;
    unsigned int VB;
} GLQuadRenderer;

typedef struct GLInstancedQuadRenderer
{
    unsigned int VA;
    unsigned int VB;
    unsigned int instanceBuffer;
} GLInstancedQuadRenderer;

#ifdef _DEBUG
void rendererGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
    GLsizei length, const GLchar* message, const void* userParam);
#endif

unsigned int genVA();
unsigned int genVB();
unsigned int genIB();

int retrieveUniformLocation(unsigned int shader, const char* name);

void drawElements(unsigned int VA, GLsizei count, GLenum IBType);
void drawInstances(unsigned int VA, GLsizei vertexCount, GLsizei instanceCount, GLenum IBType);

void moveDataWithinGLBuffer(GLenum bufferType, unsigned int buffer,
    GLintptr dstOffset, GLintptr srcOffset, GLsizeiptr size);
void moveObjectsWithinGLBuffer(GLenum bufferType, unsigned int buffer,
    size_t dstIndex, size_t srcIndex, size_t count, size_t objSize);
void eraseObjectFromGLBuffer(GLenum bufferType, unsigned int buffer,
    size_t index, size_t objectCount, size_t objSize);

unsigned int createQuadIB(size_t count, GLenum usage);

void freeGLQuadRenderer(const GLQuadRenderer* quad);
void freeGLInstancedQuadRenderer(const GLInstancedQuadRenderer* quad);
