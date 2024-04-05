#include "rendering.h"

#define INCBIN_PREFIX
#include <incbin.h>

#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "log.h"
#include "board.h"
#include "shader.h"

#include "defines.h"

INCTXT(commonShaderSrc, "../shaders/common.glsl");
INCTXT(blockVertexShaderSrc, "../shaders/block.vert");
INCTXT(blockFragmentShaderSrc, "../shaders/block.frag");
INCTXT(paddleVertexShaderSrc, "../shaders/paddle.vert");
INCTXT(paddleFragmentShaderSrc, "../shaders/paddle.frag");
INCTXT(ballVertexShaderSrc, "../shaders/ball.vert");
INCTXT(ballFragmentShaderSrc, "../shaders/ball.frag");

#ifdef _DEBUG
void GLDebugCallback(GLenum /*source*/, GLenum /*type*/, GLuint /*id*/, GLenum severity,
    GLsizei /*length*/, const GLchar* message, const void* /*userParam*/)
{
    if (ARKANOID_GL_DEBUG_MESSAGE_MIN_SEVERITY != GL_DEBUG_SEVERITY_NOTIFICATION &&
        severity > ARKANOID_GL_DEBUG_MESSAGE_MIN_SEVERITY)
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
    GLBuffers buffers = {
        .VA = genVA(),
        .VB = createBlockVB(block, GL_DYNAMIC_DRAW),
        .IB = createBlockIB(GL_STATIC_DRAW),
    };

    setBlockVertexAttributes();

    return buffers;
}

GLBuffers createNormalizedBlocksGLBuffers(const Block* blocks, size_t blockCount)
{
    GLBuffers buffers = {
        .VA = genVA(),
        .VB = createNormalizedBlocksVB(blocks, blockCount, GL_DYNAMIC_DRAW),
        .IB = createBlocksIB(blockCount, GL_STATIC_DRAW),
    };

    setBlockVertexAttributes();

    return buffers;
}

GLBuffers createBallGLBuffers(const Ball* ball)
{
    GLBuffers buffers = {
        .VA = genVA(),
        .VB = createBallVB(ball, GL_DYNAMIC_DRAW),
        .IB = createBlockIB(GL_STATIC_DRAW),
    };

    setBallVertexAttributes();

    return buffers;
}

void freeGLBuffers(const GLBuffers* buffers)
{
    glDeleteVertexArrays(1, &buffers->VA);
    glDeleteBuffers(1, &buffers->VB);
    glDeleteBuffers(1, &buffers->IB);
}

void getBlockVertices(float* vertices, const Block* block)
{
    static_assert(FLOATS_PER_BLOCK_VERTEX == 2);

    float x1 = block->position.x;
    float x2 = block->position.x + block->width;
    float y1 = block->position.y;
    float y2 = block->position.y - block->height;

    vertices[0] = x1; vertices[1] = y1;
    vertices[2] = x2; vertices[3] = y1;
    vertices[4] = x2; vertices[5] = y2;
    vertices[6] = x1; vertices[7] = y2;
}

void getNormalizedBlockVertices(float* vertices, const Block* block)
{
    static_assert(FLOATS_PER_BLOCK_VERTEX == 2);

    float normalizedX1 = normalizeCoordinate(block->position.x);
    float normalizedY1 = normalizeCoordinate(block->position.y);
    float normalizedX2 = normalizedX1 + normalizeLength(block->width);
    float normalizedY2 = normalizedY1 - normalizeLength(block->height);

    vertices[0] = normalizedX1; vertices[1] = normalizedY1;
    vertices[2] = normalizedX2; vertices[3] = normalizedY1;
    vertices[4] = normalizedX2; vertices[5] = normalizedY2;
    vertices[6] = normalizedX1; vertices[7] = normalizedY2;
}

void getBallVertices(float* vertices, const Ball* ball)
{
    static_assert(FLOATS_PER_BALL_VERTEX == 2);

    float x1 = ball->position.x - ball->radius;
    float x2 = ball->position.x + ball->radius;
    float y1 = ball->position.y - ball->radius;
    float y2 = ball->position.y + ball->radius;

    vertices[0] = x1; vertices[1] = y1;
    vertices[2] = x2; vertices[3] = y1;
    vertices[4] = x2; vertices[5] = y2;
    vertices[6] = x1; vertices[7] = y2;
}

unsigned int createBlockVB(const Block* block, GLenum usage)
{
    static_assert(FLOATS_PER_BLOCK_VERTEX == 2);

    unsigned int VB = genVB();

    float vertices[FLOATS_PER_BLOCK_VERTEX * 4];
    getBlockVertices(vertices, block);

    glBufferData(GL_ARRAY_BUFFER, BLOCK_VERTICES_SIZE, vertices, usage);

    return VB;
}

unsigned int createNormalizedBlocksVB(const Block* blocks, size_t count, GLenum usage)
{
    static_assert(FLOATS_PER_BLOCK_VERTEX == 2);

    unsigned int VB = genVB();

    float* vertices = malloc(BLOCK_VERTICES_SIZE * count);

    for (size_t i = 0; i < count; i++)
        getNormalizedBlockVertices(vertices + 4 * FLOATS_PER_BLOCK_VERTEX * i, &blocks[i]);

    glBufferData(GL_ARRAY_BUFFER, (GLsizei)BLOCK_VERTICES_SIZE * (GLsizei)count, vertices, usage);

    free(vertices);

    return VB;
}

unsigned int createBallVB(const Ball* ball, GLenum usage)
{
    static_assert(FLOATS_PER_BALL_VERTEX == 2);

    unsigned int VB = genVB();

    float vertices[FLOATS_PER_BALL_VERTEX * 4];
    getBallVertices(vertices, ball);

    glBufferData(GL_ARRAY_BUFFER, BALL_VERTICES_SIZE, vertices, usage);

    return VB;
}

void updateBlockVB(const Block* block, unsigned int blockVB)
{
    static_assert(FLOATS_PER_BLOCK_VERTEX == 2);

    float vertices[FLOATS_PER_BLOCK_VERTEX * 4];
    getBlockVertices(vertices, block);

    glBindBuffer(GL_ARRAY_BUFFER, blockVB);
    glBufferSubData(GL_ARRAY_BUFFER, 0, BLOCK_VERTICES_SIZE, vertices);
}

void updateBlocksVBOnBlocksDestroyed(unsigned int blocksVB, size_t destroyedIndex, size_t destroyedCount, size_t newBlockCount)
{
    size_t blocksToMove = newBlockCount - destroyedIndex;
    moveObjectsWithinGLBuffer(GL_ARRAY_BUFFER, blocksVB, destroyedIndex,
        destroyedIndex + destroyedCount, blocksToMove, BLOCK_VERTICES_SIZE);
}

void updateBallVB(const Ball* ball, unsigned int ballVB)
{
    static_assert(FLOATS_PER_BALL_VERTEX == 2);

    float vertices[FLOATS_PER_BALL_VERTEX * 4];
    getBallVertices(vertices, ball);

    glBindBuffer(GL_ARRAY_BUFFER, ballVB);
    glBufferSubData(GL_ARRAY_BUFFER, 0, BALL_VERTICES_SIZE, vertices);
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
    unsigned short* indices = malloc(dataSize);

    unsigned short vertexOffset = 0;

    for (unsigned short i = 0; i < count * 6; i += 6)
    {
        // first triangle
        indices[i + 0] = vertexOffset + 0;
        indices[i + 1] = vertexOffset + 1;
        indices[i + 2] = vertexOffset + 2;

        // second triangle
        indices[i + 3] = vertexOffset + 0;
        indices[i + 4] = vertexOffset + 2;
        indices[i + 5] = vertexOffset + 3;

        vertexOffset += 4;
    }

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizei)dataSize, indices, usage);

    free(indices);

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
    BallShaderUnifs unifs = {
        .normalBallCenter = retrieveUniformLocation(ballShader, "normalBallCenter"),
        .normalBallRadiusSquared = retrieveUniformLocation(ballShader, "normalBallRadiusSquared"),
    };

    return unifs;
}

void updateBallShaderUnifs(const BallShaderUnifs* unifs, const Ball* ball)
{
    glUniform2f(unifs->normalBallCenter, normalizeCoordinate(ball->position.x), normalizeCoordinate(ball->position.y));
    glUniform1f(unifs->normalBallRadiusSquared, (float)pow(normalizeLength(ball->radius), 2));
}

GameShaders createGameShaders()
{
    setCommonShaderSrc(commonShaderSrcData);

    GameShaders gameShaders = {
        .paddleShader = createShader(paddleVertexShaderSrcData,
            paddleFragmentShaderSrcData, ARKANOID_GL_SHADER_VERSION_DECL),
        .blockShader = createShader(blockVertexShaderSrcData,
            blockFragmentShaderSrcData, ARKANOID_GL_SHADER_VERSION_DECL),
        .ballShader = createShader(ballVertexShaderSrcData,
            ballFragmentShaderSrcData, ARKANOID_GL_SHADER_VERSION_DECL),
    };

    return gameShaders;
}

RenderingData createRenderingData(const GameObjects* gameObjects)
{
    RenderingData data;

    data.shaders = createGameShaders();
    data.ballShaderUnifs = retrieveBallShaderUnifs(data.shaders.ballShader);
    data.paddleBuffers = createBlockGLBuffers(&gameObjects->paddle);
    data.blocksBuffers = createNormalizedBlocksGLBuffers(gameObjects->blocks, gameObjects->blockCount);
    data.ballBuffers = createBallGLBuffers(&gameObjects->ball);
    data.blocksToRender = gameObjects->blockCount;

    return data;
}

void updateRenderingData(RenderingData* renderingData, const GameObjects* gameObjects)
{
    renderingData->blocksToRender = gameObjects->blockCount;
    updateBlockVB(&gameObjects->paddle, renderingData->paddleBuffers.VB);
    updateBallVB(&gameObjects->ball, renderingData->ballBuffers.VB);
}

void freeGameShaders(const GameShaders* shaders)
{
    glDeleteProgram(shaders->paddleShader);
    glDeleteProgram(shaders->blockShader);
    glDeleteProgram(shaders->ballShader);
}

void freeRenderingData(const RenderingData* renderingData)
{
    freeGameShaders(&renderingData->shaders);

    freeGLBuffers(&renderingData->paddleBuffers);
    freeGLBuffers(&renderingData->blocksBuffers);
    freeGLBuffers(&renderingData->ballBuffers);
}

void drawVertices(unsigned int VA, int count, GLenum IBType)
{
    glBindVertexArray(VA);
    glDrawElements(GL_TRIANGLES, count, IBType, NULL);
}

void drawBall(const Ball* ball, unsigned int ballShader, const BallShaderUnifs* unifs, unsigned int ballVA)
{
    glUseProgram(ballShader);
    updateBallShaderUnifs(unifs, ball);
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

void render(const RenderingData* renderingData, const GameObjects* gameObjects)
{
    drawPaddle(renderingData->shaders.paddleShader, renderingData->paddleBuffers.VA);
    drawBlocks(renderingData->blocksToRender, renderingData->shaders.blockShader, renderingData->blocksBuffers.VA);
    drawBall(&gameObjects->ball, renderingData->shaders.ballShader,
        &renderingData->ballShaderUnifs, renderingData->ballBuffers.VA);
}