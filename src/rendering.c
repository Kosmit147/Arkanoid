#include "rendering.h"

#include <glad/glad.h>

#include "board.h"
#include "defines.h"

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

unsigned int createBlockVB(Block* paddle, GLenum usage)
{
    unsigned int VB = genVB();

    float positions[BLOCK_VERTEX_FLOATS * 4] = {
        paddle->position.x, paddle->position.y,
        paddle->position.x + paddle->width, paddle->position.y,
        paddle->position.x + paddle->width, paddle->position.y - paddle->height,
        paddle->position.x, paddle->position.y - paddle->height,
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * BLOCK_VERTEX_FLOATS * 4, positions, usage);

    return VB;
}

void updateBlockVB(Block* paddle, unsigned int paddleVB)
{
    float positions[BLOCK_VERTEX_FLOATS * 4] = {
        paddle->position.x, paddle->position.y,
        paddle->position.x + paddle->width, paddle->position.y,
        paddle->position.x + paddle->width, paddle->position.y - paddle->height,
        paddle->position.x, paddle->position.y - paddle->height,
    };

    glBindBuffer(GL_ARRAY_BUFFER, paddleVB);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * BLOCK_VERTEX_FLOATS * 4, positions);
}

unsigned int createNormalizedBlockVB(Block* block, GLenum usage)
{
    unsigned int VB = genVB();

    float normalizedPositions[BLOCK_VERTEX_FLOATS * 4];
    normalizeBlockCoordinates(normalizedPositions, block);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * BLOCK_VERTEX_FLOATS * 4, normalizedPositions, usage);

    return VB;
}

unsigned int createBlockIB(GLenum usage)
{
    unsigned int IB = genIB();

    unsigned short indices[] = {
        0, 1, 2,
        0, 2, 3,
    };

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * 3 * 2, indices, usage);

    return IB;
}

void setBlockVertexAttributes()
{
    glVertexAttribPointer(0, BLOCK_VERTEX_FLOATS, GL_FLOAT, GL_FALSE, sizeof(float) * BLOCK_VERTEX_FLOATS, NULL);
    glEnableVertexAttribArray(0);
}

void drawVertices(unsigned int VA, int count, GLenum IBType)
{
    glBindVertexArray(VA);
    glDrawElements(GL_TRIANGLES, count, IBType, NULL);
}