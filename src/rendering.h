#pragma once

#include <glad/glad.h>

#include <stddef.h>

#include "entities.h"

typedef struct GLBuffers
{
    unsigned int VA, VB, IB;
} GLBuffers;

void GLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
    GLsizei length, const GLchar* message, const void* userParam);

// Generate GL Buffers
unsigned int genVA();
unsigned int genVB();
unsigned int genIB();

// Operate on GL Buffers
void moveDataWithinGLBuffer(GLenum bufferType, unsigned int buffer, GLintptr dstOffset,
    GLintptr srcOffset, GLsizeiptr size);
void moveObjectsWithinGLBuffer(GLenum bufferType, unsigned int buffer, size_t dstIndex,
    size_t srcIndex, size_t count, size_t objSize);

// Create GLBuffers
GLBuffers createBlockGLBuffers(const Block* block);
GLBuffers createNormalizedBlocksGLBuffers(const Block* blocks, size_t blockCount);
GLBuffers createBallGLBuffers(const Ball* ball);

// Free GLBuffers
void freeGLBuffers(GLBuffers* buffers);

// Create Vertex Buffers
unsigned int createBlockVB(const Block* block, GLenum usage);
unsigned int createNormalizedBlockVB(const Block* block, GLenum usage);
unsigned int createNormalizedBlocksVB(const Block* blocks, size_t count, GLenum usage);
unsigned int createBallVB(const Ball* ball, GLenum usage);

// Update Vertex Buffers
void updateBlockVB(const Block* block, unsigned int blockVB);
void updateBlocksVBOnBlockDestroyed(unsigned int blocksVB, size_t destroyedIndex, size_t newBlockCount);

// Create Index Buffers
unsigned int createBlockIB(GLenum usage);
unsigned int createBlocksIB(size_t count, GLenum usage);

// Set Vertex Attributes
void setBlockVertexAttributes();

// Draw Vertices
void drawVertices(unsigned int VA, int count, GLenum IBType);