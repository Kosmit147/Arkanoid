#pragma once

#include <glad/glad.h>

#include "entities.h"

void bufferBlockGLData(Block* block, float* positions, GLenum usage);
void drawBlock(Block* block);
void updateBlockVB(Block* block);