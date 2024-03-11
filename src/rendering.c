#include "rendering.h"

#include <glad/glad.h>

#include "board.h"

void bufferBlockGLData(Block* block, float* positions, GLenum usage)
{
    glGenBuffers(1, &block->glVB);
    glBindBuffer(GL_ARRAY_BUFFER, block->glVB);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * 2, positions, usage);
}

void setBlockVertexAttributes()
{
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, NULL);
    glEnableVertexAttribArray(0);
}

void drawBlock(Block* block)
{
    glBindBuffer(GL_ARRAY_BUFFER, block->glVB);
    setBlockVertexAttributes();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, NULL);
}

void updateBlockVB(Block* block)
{
    float newNormalizedPositions[4 * 2];

    normalizeBlockCoordinates(newNormalizedPositions, block->position, block->width, block->height);

    glBindBuffer(GL_ARRAY_BUFFER, block->glVB);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 4 * 2, newNormalizedPositions);
}

unsigned int genVA()
{
    unsigned int VA;
    glGenVertexArrays(1, &VA);
    glBindVertexArray(VA);

    return VA;
}

unsigned int genVB()
{
    unsigned int VB;
    glGenBuffers(1, &VB);
    glBindBuffer(GL_ARRAY_BUFFER, VB);

    return VB;
}

unsigned int genIB()
{
    unsigned int IB;
    glGenBuffers(1, &IB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);

    return IB;
}

unsigned int genRectangleIB(GLenum usage)
{
    static const unsigned char indices[] = {
        0, 1, 2,
        0, 2, 3,
    };

    unsigned int rectIB;
    glGenBuffers(1, &rectIB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rectIB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned char) * 2 * 3, indices, usage);

    return rectIB;
}