#include "rendering.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "log.h"
#include "board.h"

#include "defines.h"

#ifdef _DEBUG
void GLDebugCallback(GLenum /*source*/, GLenum /*type*/, GLuint /*id*/, GLenum severity,
    GLsizei /*length*/, const GLchar* message, const void* /*userParam*/)
{
    if (severity > ARKANOID_GL_DEBUG_MESSAGE_MIN_SEVERITY)
        return;

    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION &&
        ARKANOID_GL_DEBUG_MESSAGE_MIN_SEVERITY != GL_DEBUG_SEVERITY_NOTIFICATION)
    {
        return;
    }

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        logNotification("[OpenGL Notification]: %s.\n", message);
        break;
    case GL_DEBUG_SEVERITY_LOW:
    case GL_DEBUG_SEVERITY_MEDIUM:
        logWarning("[OpenGL Warning]: %s.\n", message);
        break;
    case GL_DEBUG_SEVERITY_HIGH:
        logError("[OpenGL Error]: %s.\n", message);
        break;
    }
}
#endif

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

void moveDataWithinGLBuffer(GLenum bufferType, unsigned int buffer, GLintptr dstOffset,
    GLintptr srcOffset, GLsizeiptr size)
{
    void* tmpData = malloc((size_t)size);

    glBindBuffer(bufferType, buffer);
    glGetBufferSubData(bufferType, srcOffset, size, tmpData);
    glBufferSubData(bufferType, dstOffset, size, tmpData);

    free(tmpData);
}

void moveObjectsWithinGLBuffer(GLenum bufferType, unsigned int buffer, size_t dstIndex,
    size_t srcIndex, size_t count, size_t objSize)
{
    GLintptr srcOffset = (GLintptr)(objSize * srcIndex);
    GLintptr dstOffset = (GLintptr)(objSize * dstIndex);
    GLsizeiptr dataSize = (GLsizeiptr)(objSize * count);

    moveDataWithinGLBuffer(bufferType, buffer, dstOffset, srcOffset, dataSize);
}

GLBuffers createBlockGLBuffers(const Block* block)
{
    GLBuffers buffers;

    buffers.VA = genVA();
    buffers.VB = createBlockVB(block, GL_DYNAMIC_DRAW);
    buffers.IB = createBlockIB(GL_STATIC_DRAW);
    setBlockVertexAttributes();

    return buffers;
}

GLBuffers createNormalizedBlocksGLBuffers(const Block* blocks, size_t blockCount)
{
    GLBuffers buffers;

    buffers.VA = genVA();
    buffers.VB = createNormalizedBlocksVB(blocks, blockCount, GL_DYNAMIC_DRAW);
    buffers.IB = createBlocksIB(blockCount, GL_STATIC_DRAW);
    setBlockVertexAttributes();

    return buffers;
}

GLBuffers createBallGLBuffers(const Ball* ball)
{
    GLBuffers buffers;

    buffers.VA = genVA();
    buffers.VB = createBallVB(ball, GL_DYNAMIC_DRAW);
    buffers.IB = createBlockIB(GL_STATIC_DRAW);
    setBallVertexAttributes();

    return buffers;
}

void freeGLBuffers(GLBuffers* buffers)
{
    glDeleteVertexArrays(1, &buffers->VA);
    glDeleteBuffers(1, &buffers->VB);
    glDeleteBuffers(1, &buffers->IB);
}

unsigned int createBlockVB(const Block* block, GLenum usage)
{
    unsigned int VB = genVB();

    static_assert(FLOATS_PER_BLOCK_VERTEX == 2);

    float x1 = block->position.x;
    float x2 = block->position.x + block->width;
    float y1 = block->position.y;
    float y2 = block->position.y - block->height;

    float positions[FLOATS_PER_BLOCK_VERTEX * 4] = {
        x1, y1,
        x2, y1,
        x2, y2,
        x1, y2,
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * FLOATS_PER_BLOCK_VERTEX * 4, positions, usage);

    return VB;
}

unsigned int createNormalizedBlockVB(const Block* block, GLenum usage)
{
    unsigned int VB = genVB();

    static_assert(FLOATS_PER_BLOCK_VERTEX == 2);

    float normalizedPositions[FLOATS_PER_BLOCK_VERTEX * 4];
    normalizeBlockCoordinates(normalizedPositions, block);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * FLOATS_PER_BLOCK_VERTEX * 4, normalizedPositions, usage);

    return VB;
}

unsigned int createNormalizedBlocksVB(const Block* blocks, size_t count, GLenum usage)
{
    unsigned int VB = genVB();

    size_t stride = sizeof(float) * 4 * FLOATS_PER_BLOCK_VERTEX;
    float* positions = malloc(stride * count);

    static_assert(FLOATS_PER_BLOCK_VERTEX == 2);

    for (size_t i = 0; i < count; i++)
        normalizeBlockCoordinates(positions + 4 * FLOATS_PER_BLOCK_VERTEX * i, &blocks[i]);

    glBufferData(GL_ARRAY_BUFFER, (GLsizei)stride * (GLsizei)count, positions, usage);
    free(positions);

    return VB;
}

unsigned int createBallVB(const Ball* ball, GLenum usage)
{
    unsigned int VB = genVB();

    static_assert(FLOATS_PER_BALL_VERTEX == 2);

    float x1 = ball->position.x - ball->radius;
    float x2 = ball->position.x + ball->radius;
    float y1 = ball->position.y - ball->radius;
    float y2 = ball->position.y + ball->radius;

    float positions[FLOATS_PER_BALL_VERTEX * 4] = {
        x1, y1,
        x2, y1,
        x2, y2,
        x1, y2,
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * FLOATS_PER_BALL_VERTEX * 4, positions, usage);

    return VB;
}

void updateBlockVB(const Block* block, unsigned int paddleVB)
{
    static_assert(FLOATS_PER_BLOCK_VERTEX == 2);

    float x1 = block->position.x;
    float x2 = block->position.x + block->width;
    float y1 = block->position.y;
    float y2 = block->position.y - block->height;

    float positions[FLOATS_PER_BLOCK_VERTEX * 4] = {
        x1, y1,
        x2, y1,
        x2, y2,
        x1, y2,
    };

    glBindBuffer(GL_ARRAY_BUFFER, paddleVB);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * FLOATS_PER_BLOCK_VERTEX * 4, positions);
}

void updateBallVB(const Ball* ball, unsigned int ballVB)
{
    static_assert(FLOATS_PER_BALL_VERTEX == 2);

    float x1 = ball->position.x - ball->radius;
    float x2 = ball->position.x + ball->radius;
    float y1 = ball->position.y - ball->radius;
    float y2 = ball->position.y + ball->radius;

    float positions[FLOATS_PER_BALL_VERTEX * 4] = {
        x1, y1,
        x2, y1,
        x2, y2,
        x1, y2,
    };

    glBindBuffer(GL_ARRAY_BUFFER, ballVB);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * FLOATS_PER_BALL_VERTEX * 4, positions);
}

void updateBlocksVBOnBlockDestroyed(unsigned int blocksVB, size_t destroyedIndex, size_t newBlockCount)
{
    size_t blocksToMoveCount = newBlockCount - destroyedIndex;
    moveObjectsWithinGLBuffer(GL_ARRAY_BUFFER, blocksVB, destroyedIndex,
        destroyedIndex + 1, blocksToMoveCount, BLOCK_VERTICES_SIZE);
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

unsigned int createBlocksIB(size_t count, GLenum usage)
{
    unsigned int IB = genIB();

    size_t dataSize = sizeof(unsigned short) * 2 * 3 * count;
    unsigned short* positions = malloc(dataSize);

    unsigned short vertexOffset = 0;

    for (unsigned short i = 0; i < count * 6; i += 6)
    {
        // first triangle
        positions[i + 0] = vertexOffset + 0;
        positions[i + 1] = vertexOffset + 1;
        positions[i + 2] = vertexOffset + 2;

        // second triangle
        positions[i + 3] = vertexOffset + 0;
        positions[i + 4] = vertexOffset + 2;
        positions[i + 5] = vertexOffset + 3;

        vertexOffset += 4;
    }

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizei)dataSize, positions, usage);
    free(positions);

    return IB;
}

void setBlockVertexAttributes()
{
    static_assert(FLOATS_PER_BLOCK_VERTEX == 2);

    glVertexAttribPointer(0, FLOATS_PER_BLOCK_VERTEX, GL_FLOAT, GL_FALSE, sizeof(float) * FLOATS_PER_BLOCK_VERTEX, NULL);
    glEnableVertexAttribArray(0);
}

void setBallVertexAttributes()
{
    static_assert(FLOATS_PER_BALL_VERTEX == 2);

    glVertexAttribPointer(0, FLOATS_PER_BALL_VERTEX, GL_FLOAT, GL_FALSE, sizeof(float) * FLOATS_PER_BALL_VERTEX, NULL);
    glEnableVertexAttribArray(0);
}

int retrieveUniformLocation(unsigned int shader, const char* name)
{
    int location = glGetUniformLocation(shader, name);

#ifdef _DEBUG
    if (location == -1)
        logWarning("[OpenGL Warning]: Uniform %s in shader %d does not exist!\n", name, shader);
#endif

    return location;
}

BallShaderUnifs retrieveBallShaderUnifs(unsigned int ballShader)
{
    BallShaderUnifs unifs;

    unifs.normalBallCenter = retrieveUniformLocation(ballShader, "normalBallCenter");
    unifs.normalBallRadiusSquared = retrieveUniformLocation(ballShader, "normalBallRadiusSquared");

    return unifs;
}

void drawVertices(unsigned int VA, int count, GLenum IBType)
{
    glBindVertexArray(VA);
    glDrawElements(GL_TRIANGLES, count, IBType, NULL);
}

void drawBall(const Ball* ball, const BallShaderUnifs* unifs, unsigned int shader, unsigned int ballVA)
{
    glUseProgram(shader);

    glUniform2f(unifs->normalBallCenter, normalizeCoordinate(ball->position.x), normalizeCoordinate(ball->position.y));
    glUniform1f(unifs->normalBallRadiusSquared, (float)pow(normalizeLength(ball->radius), 2));

    drawVertices(ballVA, 6, GL_UNSIGNED_SHORT);
}

void drawPaddle(unsigned int shader, unsigned int paddleVA)
{
    glUseProgram(shader);
    drawVertices(paddleVA, 6, GL_UNSIGNED_SHORT);
}

void drawBlocks(size_t blockCount, unsigned int shader, unsigned int blocksVA)
{
    glUseProgram(shader);
    drawVertices(blocksVA, (int)blockCount * 6, GL_UNSIGNED_SHORT);
}