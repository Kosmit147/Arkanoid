#include "rendering.h"

#include <glad/glad.h>

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "log.h"
#include "board.h"

#include "defines.h"

void GLDebugCallback(GLenum /*source*/, GLenum /*type*/, GLuint /*id*/, GLenum severity,
    GLsizei /*length*/, const GLchar* message, const void* /*userParam*/)
{
    if (severity > GL_DEBUG_MESSAGE_MIN_SEVERITY)
        return;

    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION && GL_DEBUG_MESSAGE_MIN_SEVERITY != GL_DEBUG_SEVERITY_NOTIFICATION)
        return;

    const char* color;

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        color = ANSI_COLOR_WHITE;
        break;
    case GL_DEBUG_SEVERITY_LOW:
        color = ANSI_COLOR_YELLOW;
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        color = ANSI_COLOR_YELLOW;
        break;
    case GL_DEBUG_SEVERITY_HIGH:
        color = ANSI_COLOR_RED;
        break;
    }

    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
        logNotification("[OpenGL Debug Message]: %s.\n", message, color);
    else
        logError("[OpenGL Error Message]: %s.\n", message, color);
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

GLBuffers createBlockGLBuffers(Block* block)
{
    GLBuffers buffers;

    buffers.VA = genVA();
    buffers.VB = createBlockVB(block, GL_DYNAMIC_DRAW);
    buffers.IB = createBlockIB(GL_STATIC_DRAW);
    setBlockVertexAttributes();

    return buffers;
}

GLBuffers createBallGLBuffers(const Ball* ball)
{
    GLBuffers buffers;

    buffers.VA = genVA();
    buffers.VB = createBallVB(ball, GL_DYNAMIC_DRAW);
    buffers.IB = createBlockIB(GL_STATIC_DRAW);
    setBlockVertexAttributes();

    return buffers;
}

GLBuffers createNormalizedBlocksGLBuffers(Block* blocks, size_t blockCount)
{
    GLBuffers buffers;

    buffers.VA = genVA();
    buffers.VB = createNormalizedBlocksVB(blocks, blockCount, GL_DYNAMIC_DRAW);
    buffers.IB = createBlocksIB(blockCount, GL_STATIC_DRAW);
    setBlockVertexAttributes();

    return buffers;
}

unsigned int createBlockVB(Block* block, GLenum usage)
{
    unsigned int VB = genVB();

    // if BLOCK_VERTEX_FLOATS changed, we have to update this code
    static_assert(FLOATS_PER_BLOCK_VERTEX == 2);

    float positions[FLOATS_PER_BLOCK_VERTEX * 4] = {
        block->position.x, block->position.y,
        block->position.x + block->width, block->position.y,
        block->position.x + block->width, block->position.y - block->height,
        block->position.x, block->position.y - block->height,
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * FLOATS_PER_BLOCK_VERTEX * 4, positions, usage);

    return VB;
}

unsigned int createBallVB(const Ball* ball, GLenum usage)
{
    unsigned int VB = genVB();

    // if BLOCK_VERTEX_FLOATS changed, we have to update this code
    static_assert(FLOATS_PER_BLOCK_VERTEX == 2);

    float positions[FLOATS_PER_BLOCK_VERTEX * 4] = {
        ball->position.x - ball->radius, ball->position.y - ball->radius,
        ball->position.x + ball->radius, ball->position.y - ball->radius,
        ball->position.x + ball->radius, ball->position.y + ball->radius,
        ball->position.x - ball->radius, ball->position.y + ball->radius,
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * FLOATS_PER_BLOCK_VERTEX * 4, positions, usage);

    return VB;
}

void updateBlockVB(Block* block, unsigned int paddleVB)
{
    // if BLOCK_VERTEX_FLOATS changed, we have to update this code
    static_assert(FLOATS_PER_BLOCK_VERTEX == 2);

    float positions[FLOATS_PER_BLOCK_VERTEX * 4] = {
        block->position.x, block->position.y,
        block->position.x + block->width, block->position.y,
        block->position.x + block->width, block->position.y - block->height,
        block->position.x, block->position.y - block->height,
    };

    glBindBuffer(GL_ARRAY_BUFFER, paddleVB);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * FLOATS_PER_BLOCK_VERTEX * 4, positions);
}

unsigned int createNormalizedBlockVB(Block* block, GLenum usage)
{
    unsigned int VB = genVB();

    // if BLOCK_VERTEX_FLOATS changed, we have to update this code
    static_assert(FLOATS_PER_BLOCK_VERTEX == 2);

    float normalizedPositions[FLOATS_PER_BLOCK_VERTEX * 4];
    normalizeBlockCoordinates(normalizedPositions, block);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * FLOATS_PER_BLOCK_VERTEX * 4, normalizedPositions, usage);

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

    size_t stride = sizeof(float) * 4 * FLOATS_PER_BLOCK_VERTEX;
    float* positions = malloc(stride * count);

    // if BLOCK_VERTEX_FLOATS changed, we have to update this code
    static_assert(FLOATS_PER_BLOCK_VERTEX == 2);

    for (size_t i = 0; i < count; i++)
        normalizeBlockCoordinates(positions + 4 * FLOATS_PER_BLOCK_VERTEX * i, &blocks[i]);

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
    glVertexAttribPointer(0, FLOATS_PER_BLOCK_VERTEX, GL_FLOAT, GL_FALSE, sizeof(float) * FLOATS_PER_BLOCK_VERTEX, NULL);
    glEnableVertexAttribArray(0);
}

void drawVertices(unsigned int VA, int count, GLenum IBType)
{
    glBindVertexArray(VA);
    glDrawElements(GL_TRIANGLES, count, IBType, NULL);
}

void freeGLBuffers(GLBuffers* buffers)
{
    glDeleteVertexArrays(1, &buffers->VA);
    glDeleteBuffers(1, &buffers->VB);
    glDeleteBuffers(1, &buffers->IB);
}