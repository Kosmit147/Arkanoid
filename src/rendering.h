#pragma once

#include <glad/glad.h>

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

GLBuffers createBlockGLBuffers(Block* block);
GLBuffers createNormalizedBlocksGLBuffers(Block* blocks, size_t blockCount);

unsigned int createBlockVB(Block* block, GLenum usage);
void updateBlockVB(Block* block, unsigned int blockVB);
unsigned int createNormalizedBlockVB(Block* block, GLenum usage);
unsigned int createBlockIB(GLenum usage);

unsigned int createNormalizedBlocksVB(Block* blocks, size_t count, GLenum usage);
unsigned int createBlocksIB(size_t count, GLenum usage);

void setBlockVertexAttributes();

void drawVertices(unsigned int VA, int count, GLenum IBType);

void freeGLBuffers(GLBuffers* buffers);