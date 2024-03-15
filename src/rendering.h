#pragma once

#include <glad/glad.h>

#include "entities.h"

unsigned int genVA();
unsigned int genVB();
unsigned int genIB();

unsigned int createBlockVB(Block* paddle, GLenum usage);
void updateBlockVB(Block* paddle, unsigned int paddleVB);
unsigned int createNormalizedBlockVB(Block* block, GLenum usage);
unsigned int createBlockIB(GLenum usage);

/* TODO
unsigned int createNormalizedBlocksVB(Block* blocks, size_t count, GLenum usage);
unsigned int createBlocksIB(size_t count, GLenum usage);
*/

void setBlockVertexAttributes();

void drawVertices(unsigned int VA, unsigned int count, GLenum IBType);