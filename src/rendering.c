#include "rendering.h"

#include <glad/glad.h>

#include <stdlib.h>
#include <assert.h>

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

unsigned int createBlockVB(Block* block, GLenum usage)
{
    unsigned int VB = genVB();

    // if BLOCK_VERTEX_FLOATS changed, we have to update this code
    static_assert(BLOCK_VERTEX_FLOATS == 2);

    float positions[BLOCK_VERTEX_FLOATS * 4] = {
        block->position.x, block->position.y,
        block->position.x + block->width, block->position.y,
        block->position.x + block->width, block->position.y - block->height,
        block->position.x, block->position.y - block->height,
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * BLOCK_VERTEX_FLOATS * 4, positions, usage);

    return VB;
}

void updateBlockVB(Block* block, unsigned int paddleVB)
{
    // if BLOCK_VERTEX_FLOATS changed, we have to update this code
    static_assert(BLOCK_VERTEX_FLOATS == 2);

    float positions[BLOCK_VERTEX_FLOATS * 4] = {
        block->position.x, block->position.y,
        block->position.x + block->width, block->position.y,
        block->position.x + block->width, block->position.y - block->height,
        block->position.x, block->position.y - block->height,
    };

    glBindBuffer(GL_ARRAY_BUFFER, paddleVB);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * BLOCK_VERTEX_FLOATS * 4, positions);
}

unsigned int createNormalizedBlockVB(Block* block, GLenum usage)
{
    unsigned int VB = genVB();

    // if BLOCK_VERTEX_FLOATS changed, we have to update this code
    static_assert(BLOCK_VERTEX_FLOATS == 2);

    float normalizedPositions[BLOCK_VERTEX_FLOATS * 4];
    normalizeBlockCoordinates(normalizedPositions, block);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * BLOCK_VERTEX_FLOATS * 4, normalizedPositions, usage);

    return VB;
}

unsigned int createBlockIB(GLenum usage)
{
    unsigned int IB = genIB();

    static const unsigned short indices[] = {
        0, 1, 2,
        0, 2, 3,
    };

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * 3 * 2, indices, usage);

    return IB;
}

unsigned int createNormalizedBlocksVB(Block* blocks, size_t count, GLenum usage)
{
    unsigned int VB = genVB();

    size_t stride = sizeof(float) * 4 * BLOCK_VERTEX_FLOATS;
    float* positions = malloc(stride * count);

    // if BLOCK_VERTEX_FLOATS changed, we have to update this code
    static_assert(BLOCK_VERTEX_FLOATS == 2);

    for (size_t i = 0; i < count; i++)
        normalizeBlockCoordinates(positions + 4 * BLOCK_VERTEX_FLOATS * i, &blocks[i]);

    glBufferData(GL_ARRAY_BUFFER, (GLsizei)stride * (GLsizei)count, positions, usage);
    free(positions);

    return VB;
}

unsigned int createBlocksIB(size_t count, GLenum usage)
{
    unsigned int IB = genIB();

    size_t dataSize = sizeof(unsigned short) * 2 * 3 * count;
    unsigned short* positions = malloc(dataSize);

    for (unsigned short i = 0; i < count; i++)
    {
        unsigned short indexOffset = i * 2 * 3;
        unsigned short vertexOffset = i * 4;

        // first triangle
        positions[indexOffset + 0] = vertexOffset + 0; 
        positions[indexOffset + 1] = vertexOffset + 1; 
        positions[indexOffset + 2] = vertexOffset + 2;

        // second triangle
        positions[indexOffset + 3] = vertexOffset + 0; 
        positions[indexOffset + 4] = vertexOffset + 2; 
        positions[indexOffset + 5] = vertexOffset + 3;
    }

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizei)dataSize, positions, usage);
    free(positions);

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