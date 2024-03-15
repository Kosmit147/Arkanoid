#pragma once

#include <glad/glad.h>

#include "entities.h"

unsigned int genVA();
unsigned int genVB();
unsigned int genIB();

unsigned int createBlockVB(Block* block, GLenum usage);
void updateBlockVB(Block* block, unsigned int blockVB);
unsigned int createNormalizedBlockVB(Block* block, GLenum usage);
unsigned int createBlockIB(GLenum usage);

unsigned int createNormalizedBlocksVB(Block* blocks, size_t count, GLenum usage);
unsigned int createBlocksIB(size_t count, GLenum usage);

void setBlockVertexAttributes();

void drawVertices(unsigned int VA, int count, GLenum IBType);