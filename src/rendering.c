#include "rendering.h"

#include <glad/glad.h>

#define INCBIN_PREFIX
#include <incbin.h>

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

#include "shader.h"

#include "defines.h"

INCTXT(commonShaderSrc, "../shaders/common.glsl");
INCTXT(blockVertexShaderSrc, "../shaders/block.vert");
INCTXT(blockFragmentShaderSrc, "../shaders/block.frag");
INCTXT(paddleVertexShaderSrc, "../shaders/paddle.vert");
INCTXT(paddleFragmentShaderSrc, "../shaders/paddle.frag");
INCTXT(ballVertexShaderSrc, "../shaders/ball.vert");
INCTXT(ballFragmentShaderSrc, "../shaders/ball.frag");

static void getBlockVertices(float* vertices, const Block* block)
{
    static_assert(FLOATS_PER_BLOCK_VERTEX == 2, "Expected FLOATS_PER_BLOCK_VERTEX == 2");

    float x1 = block->position.x;
    float x2 = block->position.x + block->width;
    float y1 = block->position.y;
    float y2 = block->position.y - block->height;

    vertices[0] = x1; vertices[1] = y1;
    vertices[2] = x2; vertices[3] = y1;
    vertices[4] = x2; vertices[5] = y2;
    vertices[6] = x1; vertices[7] = y2;
}

static unsigned int createBlockVB(const Block* block, GLenum usage)
{
    static_assert(FLOATS_PER_BLOCK_VERTEX == 2, "Expected FLOATS_PER_BLOCK_VERTEX == 2");

    unsigned int VB = genVB();

    float vertices[FLOATS_PER_BLOCK_VERTEX * 4];
    getBlockVertices(vertices, block);

    glBufferData(GL_ARRAY_BUFFER, BLOCK_VERTICES_SIZE, vertices, usage);

    return VB;
}

static void getNormalizedBlockVertices(float* vertices, const Block* block)
{
    static_assert(FLOATS_PER_BLOCK_VERTEX == 2, "Expected FLOATS_PER_BLOCK_VERTEX == 2");

    float normalizedX1 = normalizeCoordinate(block->position.x);
    float normalizedY1 = normalizeCoordinate(block->position.y);
    float normalizedX2 = normalizedX1 + normalizeLength(block->width);
    float normalizedY2 = normalizedY1 - normalizeLength(block->height);

    vertices[0] = normalizedX1; vertices[1] = normalizedY1;
    vertices[2] = normalizedX2; vertices[3] = normalizedY1;
    vertices[4] = normalizedX2; vertices[5] = normalizedY2;
    vertices[6] = normalizedX1; vertices[7] = normalizedY2;
}

static unsigned int createNormalizedBlocksVB(const Block* blocks, size_t count, GLenum usage)
{
    static_assert(FLOATS_PER_BLOCK_VERTEX == 2, "Expected FLOATS_PER_BLOCK_VERTEX == 2");

    unsigned int VB = genVB();

    float* vertices = malloc(BLOCK_VERTICES_SIZE * count);

    for (size_t i = 0; i < count; i++)
        getNormalizedBlockVertices(vertices + 4 * FLOATS_PER_BLOCK_VERTEX * i, &blocks[i]);

    glBufferData(GL_ARRAY_BUFFER, (GLsizei)BLOCK_VERTICES_SIZE * (GLsizei)count, vertices, usage);

    free(vertices);

    return VB;
}

static void getBallVertices(float* vertices, const Ball* ball)
{
    static_assert(FLOATS_PER_BALL_VERTEX == 2, "Expected FLOATS_PER_BALL_VERTEX == 2");

    float x1 = ball->position.x - ball->radius;
    float x2 = ball->position.x + ball->radius;
    float y1 = ball->position.y - ball->radius;
    float y2 = ball->position.y + ball->radius;

    vertices[0] = x1; vertices[1] = y1;
    vertices[2] = x2; vertices[3] = y1;
    vertices[4] = x2; vertices[5] = y2;
    vertices[6] = x1; vertices[7] = y2;
}

static unsigned int createBallVB(const Ball* ball, GLenum usage)
{
    static_assert(FLOATS_PER_BALL_VERTEX == 2, "Expected FLOATS_PER_BALL_VERTEX == 2");

    unsigned int VB = genVB();

    float vertices[FLOATS_PER_BALL_VERTEX * 4];
    getBallVertices(vertices, ball);

    glBufferData(GL_ARRAY_BUFFER, BALL_VERTICES_SIZE, vertices, usage);

    return VB;
}

static GameShaders createGameShaders()
{
    setCommonShaderSrc(commonShaderSrcData);

    return (GameShaders) {
        .paddleShader = createShader(paddleVertexShaderSrcData,
            paddleFragmentShaderSrcData, ARKANOID_GL_SHADER_VERSION_DECL),
        .blockShader = createShader(blockVertexShaderSrcData,
            blockFragmentShaderSrcData, ARKANOID_GL_SHADER_VERSION_DECL),
        .ballShader = createShader(ballVertexShaderSrcData,
            ballFragmentShaderSrcData, ARKANOID_GL_SHADER_VERSION_DECL),
    };
}

static BallShaderUnifs retrieveBallShaderUnifs(unsigned int ballShader)
{
    return (BallShaderUnifs) {
        .normalBallCenter = retrieveUniformLocation(ballShader, "normalBallCenter"),
        .normalBallRadiusSquared = retrieveUniformLocation(ballShader, "normalBallRadiusSquared"),
    };
}

static void setBlockVertexAttributes()
{
    static_assert(FLOATS_PER_BLOCK_VERTEX == 2, "Expected FLOATS_PER_BLOCK_VERTEX == 2");

    glVertexAttribPointer(0, FLOATS_PER_BLOCK_VERTEX, GL_FLOAT, GL_FALSE, sizeof(float) * FLOATS_PER_BLOCK_VERTEX, NULL);
    glEnableVertexAttribArray(0);
}

static GLQuad createBlockGLQuad(const Block* block, unsigned int quadIB)
{
    GLQuad quad = {
        .VA = genVA(),
        .VB = createBlockVB(block, GL_DYNAMIC_DRAW),
    };

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIB);
    setBlockVertexAttributes();

    return quad;
}

static GLQuad createNormalizedBlocksGLQuad(const Block* blocks, size_t blockCount, unsigned int quadIB)
{
    GLQuad quad = {
        .VA = genVA(),
        .VB = createNormalizedBlocksVB(blocks, blockCount, GL_DYNAMIC_DRAW),
    };

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIB);
    setBlockVertexAttributes();

    return quad;
}

static void setBallVertexAttributes()
{
    static_assert(FLOATS_PER_BALL_VERTEX == 2, "Expected FLOATS_PER_BALL_VERTEX == 2");

    glVertexAttribPointer(0, FLOATS_PER_BALL_VERTEX, GL_FLOAT, GL_FALSE, sizeof(float) * FLOATS_PER_BALL_VERTEX, NULL);
    glEnableVertexAttribArray(0);
}

static GLQuad createBallGLQuad(const Ball* ball, unsigned int quadIB)
{
    GLQuad quad = {
        .VA = genVA(),
        .VB = createBallVB(ball, GL_DYNAMIC_DRAW),
    };

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIB);
    setBallVertexAttributes();

    return quad;
}

void initRenderingData(GameRenderingData* data, const GameObjects* gameObjects)
{
    data->shaders = createGameShaders();
    data->ballShaderUnifs = retrieveBallShaderUnifs(data->shaders.ballShader);
    data->quadIB = createQuadIB(MAX_QUADS, GL_STATIC_DRAW);
    data->paddleQuad = createBlockGLQuad(&gameObjects->paddle, data->quadIB);
    data->blocksQuad = createNormalizedBlocksGLQuad(gameObjects->blocks, gameObjects->blockCount, data->quadIB);
    data->ballQuad = createBallGLQuad(&gameObjects->ball, data->quadIB);
}

static void updateBlockVB(const Block* block, unsigned int blockVB)
{
    static_assert(FLOATS_PER_BLOCK_VERTEX == 2, "Expected FLOATS_PER_BLOCK_VERTEX == 2");

    float vertices[FLOATS_PER_BLOCK_VERTEX * 4];
    getBlockVertices(vertices, block);

    glBindBuffer(GL_ARRAY_BUFFER, blockVB);
    glBufferSubData(GL_ARRAY_BUFFER, 0, BLOCK_VERTICES_SIZE, vertices);
}

static void updateBallVB(const Ball* ball, unsigned int ballVB)
{
    static_assert(FLOATS_PER_BALL_VERTEX == 2, "Expected FLOATS_PER_BALL_VERTEX == 2");

    float vertices[FLOATS_PER_BALL_VERTEX * 4];
    getBallVertices(vertices, ball);

    glBindBuffer(GL_ARRAY_BUFFER, ballVB);
    glBufferSubData(GL_ARRAY_BUFFER, 0, BALL_VERTICES_SIZE, vertices);
}

void updateRenderingData(GameRenderingData* renderingData, const GameObjects* gameObjects)
{
    updateBlockVB(&gameObjects->paddle, renderingData->paddleQuad.VB);
    updateBallVB(&gameObjects->ball, renderingData->ballQuad.VB);
}

static void freeGameShaders(const GameShaders* shaders)
{
    glDeleteProgram(shaders->paddleShader);
    glDeleteProgram(shaders->blockShader);
    glDeleteProgram(shaders->ballShader);
}

void freeRenderingData(const GameRenderingData* renderingData)
{
    freeGameShaders(&renderingData->shaders);

    freeGLQuad(&renderingData->paddleQuad);
    freeGLQuad(&renderingData->blocksQuad);
    freeGLQuad(&renderingData->ballQuad);

    glDeleteBuffers(1, &renderingData->quadIB);
}

static void updateBallShaderUnifs(const BallShaderUnifs* unifs, const Ball* ball)
{
    glUniform2f(unifs->normalBallCenter, normalizeCoordinate(ball->position.x),
        normalizeCoordinate(ball->position.y));
    glUniform1f(unifs->normalBallRadiusSquared, powf(normalizeLength(ball->radius), 2.0f));
}

static void drawBall(const Ball* ball, unsigned int ballShader, const BallShaderUnifs* unifs, unsigned int ballVA)
{
    glUseProgram(ballShader);
    updateBallShaderUnifs(unifs, ball);
    drawVertices(ballVA, 6, GL_UNSIGNED_SHORT);
}

static void drawPaddle(unsigned int shader, unsigned int paddleVA)
{
    glUseProgram(shader);
    drawVertices(paddleVA, 6, GL_UNSIGNED_SHORT);
}

static void drawBlocks(size_t blockCount, unsigned int shader, unsigned int blocksVA)
{
    glUseProgram(shader);
    drawVertices(blocksVA, (int)blockCount * 6, GL_UNSIGNED_SHORT);
}

void render(const GameRenderingData* renderingData, const GameObjects* gameObjects)
{
    drawPaddle(renderingData->shaders.paddleShader, renderingData->paddleQuad.VA);
    drawBlocks(gameObjects->blockCount, renderingData->shaders.blockShader,
        renderingData->blocksQuad.VA);
    drawBall(&gameObjects->ball, renderingData->shaders.ballShader,
        &renderingData->ballShaderUnifs, renderingData->ballQuad.VA);
}
