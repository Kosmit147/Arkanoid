#pragma once

#include <glad/glad.h>

#include <stddef.h>

#include "entities.h"

void GLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
    GLsizei length, const GLchar* message, const void* userParam);

typedef struct GLBuffers
{
    unsigned int VA, VB, IB;
} GLBuffers;

unsigned int genVA();
unsigned int genVB();
unsigned int genIB();

GLBuffers createBlockGLBuffers(const Block* block);
GLBuffers createNormalizedBlocksGLBuffers(const Block* blocks, size_t blockCount);

unsigned int createBlockVB(const Block* block, GLenum usage);
void updateBlockVB(const Block* block, unsigned int blockVB);
unsigned int createNormalizedBlockVB(const Block* block, GLenum usage);
unsigned int createBlockIB(GLenum usage);

unsigned int createNormalizedBlocksVB(const Block* blocks, size_t count, GLenum usage);
unsigned int createBlocksIB(size_t count, GLenum usage);

GLBuffers createBallGLBuffers(const Ball* ball);
unsigned int createBallVB(const Ball* ball, GLenum usage);
void updateBallVB(const Ball* ball, unsigned int ballVB);

void setBlockVertexAttributes();

void drawVertices(unsigned int VA, int count, GLenum IBType);

void freeGLBuffers(GLBuffers* buffers);