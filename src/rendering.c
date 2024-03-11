#include "rendering.h"

#include <glad/glad.h>

#include "board.h"

void bufferBlockGLData(Block* block, float* positions, GLenum usage)
{
    glGenBuffers(1, &block->glVB);
    glBindBuffer(GL_ARRAY_BUFFER, block->glVB);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * 2, positions, usage);
}

void drawBlock(Block* block)
{
    glBindBuffer(GL_ARRAY_BUFFER, block->glVB);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, NULL);
}

void updateBlockVB(Block* block)
{
    float newNormalizedPositions[4 * 2];

    normalizeBlockCoordinates(newNormalizedPositions, block->position, block->width, block->height);

    glBindBuffer(GL_ARRAY_BUFFER, block->glVB);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 4 * 2, newNormalizedPositions);
}