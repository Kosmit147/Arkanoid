#pragma once

#include <glad/glad.h>

#include "entities.h"

void bufferBlockGLData(Block* block, float* positions, GLenum usage);
void setBlockVertexAttributes();
void drawBlock(Block* block);
void updateBlockVB(Block* block);

unsigned int genVA();
unsigned int genVB();
unsigned int genIB();
unsigned int genRectangleIB(GLenum usage);