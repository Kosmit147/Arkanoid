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

static void getPaddleVertices(float* vertices, const Block* paddle)
{
    static_assert(FLOATS_PER_PADDLE_VERTEX == 2, "Expected FLOATS_PER_PADDLE_VERTEX == 2");

    float x1 = paddle->position.x;
    float x2 = paddle->position.x + paddle->width;
    float y1 = paddle->position.y;
    float y2 = paddle->position.y - paddle->height;

    vertices[0] = x1; vertices[1] = y1;
    vertices[2] = x2; vertices[3] = y1;
    vertices[4] = x2; vertices[5] = y2;
    vertices[6] = x1; vertices[7] = y2;
}

static unsigned int createPaddleVB(const Block* paddle)
{
    unsigned int VB = genVB();

    float vertices[FLOATS_PER_PADDLE_VERTEX * 4];
    getPaddleVertices(vertices, paddle);

    glBufferData(GL_ARRAY_BUFFER, PADDLE_VERTICES_SIZE, vertices, GL_DYNAMIC_DRAW);

    return VB;
}

static void getBlockVertices(float* vertices, const Block* block)
{
    static_assert(FLOATS_PER_BLOCK_VERTEX == 2, "Expected FLOATS_PER_BLOCK_VERTEX == 2");

    float normalizedX1 = block->position.x;
    float normalizedY1 = block->position.y;
    float normalizedX2 = normalizedX1 + normalizeLength(block->width);
    float normalizedY2 = normalizedY1 - normalizeLength(block->height);

    vertices[0] = normalizedX1; vertices[1] = normalizedY1;
    vertices[2] = normalizedX2; vertices[3] = normalizedY1;
    vertices[4] = normalizedX2; vertices[5] = normalizedY2;
    vertices[6] = normalizedX1; vertices[7] = normalizedY2;
}

static unsigned int createBlockVB(const Block* block)
{
    unsigned int VB = genVB();

    float vertices[FLOATS_PER_BLOCK_VERTEX * 4];
    getBlockVertices(vertices, block);

    glBufferData(GL_ARRAY_BUFFER, BLOCK_VERTICES_SIZE, vertices, GL_STATIC_DRAW);

    return VB;
}

static void getBlockInstanceVertices(float* vertices, const Block* block)
{
    static_assert(FLOATS_PER_BLOCK_INSTANCE_VERTEX == 10, "Expected FLOATS_PER_BLOCK_INSTANCE_VERTEX == 10");

    Vec2 translation = { .x = normalizeLength(block->position.x), .y = normalizeLength(block->position.y) };
    vertices[0] = translation.x;
    vertices[1] = translation.y;

    float normalizedBlockWidth = normalizeLength(block->width);
    float normalizedBlockHeight = normalizeLength(block->height);
    float normalizedBorderWidth = normalizeLength(BLOCK_BORDER_WIDTH);

    Vec2 borderRectTopLeft = {
        .x = translation.x + normalizedBorderWidth - 1.0f,
        .y = translation.y - normalizedBorderWidth - 1.0f,
    };

    Vec2 borderRectBottomRight = {
        .x = translation.x + normalizedBlockWidth - normalizedBorderWidth - 1.0f,
        .y = translation.y - normalizedBlockHeight + normalizedBorderWidth - 1.0f,
    };

    vertices[2] = borderRectTopLeft.x;
    vertices[3] = borderRectTopLeft.y;
    vertices[4] = borderRectBottomRight.x;
    vertices[5] = borderRectBottomRight.y;

    Vec4 color = getRandomBlockColor();
    vertices[6] = color.r;
    vertices[7] = color.g;
    vertices[8] = color.b;
    vertices[9] = color.a;
}

static unsigned int createBlocksInstanceBuffer(const Block* blocks, size_t blockCount)
{
    unsigned int instBuff = genVB();

    float* vertices = malloc(BLOCK_INSTANCE_VERTICES_SIZE * blockCount);

    for (size_t i = 0; i < blockCount; i++)
        getBlockInstanceVertices(vertices + i * FLOATS_PER_BLOCK_INSTANCE_VERTEX, &blocks[i]);

    glBufferData(GL_ARRAY_BUFFER, (GLsizei)BLOCK_INSTANCE_VERTICES_SIZE * (GLsizei)blockCount, vertices,
        GL_DYNAMIC_DRAW);

    free(vertices);

    return instBuff;
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

static unsigned int createBallVB(const Ball* ball)
{
    unsigned int VB = genVB();

    float vertices[FLOATS_PER_BALL_VERTEX * 4];
    getBallVertices(vertices, ball);

    glBufferData(GL_ARRAY_BUFFER, BALL_VERTICES_SIZE, vertices, GL_DYNAMIC_DRAW);

    return VB;
}

static PaddleShaderUnifs retrievePaddleShaderUnifs(unsigned int paddleShader)
{
    return (PaddleShaderUnifs) {
        .color = retrieveUniformLocation(paddleShader, "color"),
    };
}

static BallShaderUnifs retrieveBallShaderUnifs(unsigned int ballShader)
{
    return (BallShaderUnifs) {
        .normalBallCenter = retrieveUniformLocation(ballShader, "normalizedBallCenter"),
        .normalBallRadiusSquared = retrieveUniformLocation(ballShader, "normalizedBallRadiusSquared"),
    };
}

static GameShaders createGameShaders()
{
    GameShaders shaders;
    setCommonShaderSrc(commonShaderSrcData);

    shaders.paddleShader = createShader(paddleVertexShaderSrcData,
        paddleFragmentShaderSrcData, ARKANOID_GL_SHADER_VERSION_DECL),
    shaders.blockShader = createShader(blockVertexShaderSrcData,
        blockFragmentShaderSrcData, ARKANOID_GL_SHADER_VERSION_DECL),
    shaders.ballShader = createShader(ballVertexShaderSrcData,
        ballFragmentShaderSrcData, ARKANOID_GL_SHADER_VERSION_DECL),

    shaders.paddleShaderUnifs = retrievePaddleShaderUnifs(shaders.paddleShader);
    shaders.ballShaderUnifs = retrieveBallShaderUnifs(shaders.ballShader);

    return shaders;
}

static void setPaddleRendererVertexAttributes()
{
    static_assert(FLOATS_PER_PADDLE_VERTEX == 2, "Expected FLOATS_PER_PADDLE_VERTEX == 2");

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * FLOATS_PER_PADDLE_VERTEX, NULL);
    glEnableVertexAttribArray(0);
}

static QuadRenderer createPaddleRenderer(const Block* paddle, unsigned int quadIB)
{
    QuadRenderer quad = {
        .VA = genVA(),
        .VB = createPaddleVB(paddle),
    };

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIB);
    setPaddleRendererVertexAttributes();

    return quad;
}

static void setBlockRendererVertexAttributes(unsigned int VB, unsigned int instanceBuffer)
{
    static_assert(FLOATS_PER_BLOCK_VERTEX == 2, "Expected FLOATS_PER_BLOCK_VERTEX == 2");
    static_assert(FLOATS_PER_BLOCK_INSTANCE_VERTEX == 10, "Expected FLOATS_PER_BLOCK_INSTANCE_VERTEX == 10");

    // vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, VB);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * FLOATS_PER_BLOCK_VERTEX, NULL);
    glEnableVertexAttribArray(0);

    // instance buffer
    glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * FLOATS_PER_BLOCK_INSTANCE_VERTEX, NULL);
    glVertexAttribDivisor(1, 1);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * FLOATS_PER_BLOCK_INSTANCE_VERTEX,
        (void*)(sizeof(float) * 2));
    glVertexAttribDivisor(2, 1);
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(float) * FLOATS_PER_BLOCK_INSTANCE_VERTEX,
        (void*)(sizeof(float) * 4));
    glVertexAttribDivisor(3, 1);
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(float) * FLOATS_PER_BLOCK_INSTANCE_VERTEX,
        (void*)(sizeof(float) * 6));
    glVertexAttribDivisor(4, 1);
    glEnableVertexAttribArray(4);
}

static InstancedQuadRenderer createBlocksRenderer(const Block* blocks, size_t blockCount,
    unsigned int quadIB)
{
    Block baseBlock = {
        // start at (-1.0, -1.0), use a translation vector in the shader
        .position = (Vec2){ .x = -1.0f, .y = -1.0f },
        .width = 0.0f,
        .height = 0.0f,
    };

    // assumes all blocks have the same width and height
    if (blockCount > 0)
    {
        baseBlock.width = blocks[0].width;
        baseBlock.height = blocks[0].height;
    }

    InstancedQuadRenderer renderer = {
        .VA = genVA(),
        .VB = createBlockVB(&baseBlock),
        .instanceBuffer = createBlocksInstanceBuffer(blocks, blockCount),
    };

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIB);
    setBlockRendererVertexAttributes(renderer.VB, renderer.instanceBuffer);

    return renderer;
}

static void setBallRendererVertexAttributes()
{
    static_assert(FLOATS_PER_BALL_VERTEX == 2, "Expected FLOATS_PER_BALL_VERTEX == 2");

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * FLOATS_PER_BALL_VERTEX, NULL);
    glEnableVertexAttribArray(0);
}

static QuadRenderer createBallRenderer(const Ball* ball, unsigned int quadIB)
{
    QuadRenderer quad = {
        .VA = genVA(),
        .VB = createBallVB(ball),
    };

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIB);
    setBallRendererVertexAttributes();

    return quad;
}

static void initPaddleUnifs(const PaddleShaderUnifs* unifs)
{
    Vec4 paddleColor = getRandomPaddleColor();
    glUniform4f(unifs->color, paddleColor.r, paddleColor.g, paddleColor.b, paddleColor.a);
}

void initRenderData(GameRenderData* data, const GameObjects* gameObjects)
{
    data->shaders = createGameShaders();

    glUseProgram(data->shaders.paddleShader);
    initPaddleUnifs(&data->shaders.paddleShaderUnifs);

    data->quadIB = createQuadIB(MAX_QUADS, GL_STATIC_DRAW);
    data->paddleRenderer = createPaddleRenderer(&gameObjects->paddle, data->quadIB);
    data->blocksRenderer = createBlocksRenderer(gameObjects->blocks, gameObjects->blockCount, data->quadIB);
    data->ballRenderer = createBallRenderer(&gameObjects->ball, data->quadIB);
}

static void updatePaddleVB(const Block* paddle, unsigned int paddleVB)
{
    static_assert(FLOATS_PER_PADDLE_VERTEX == 2, "Expected FLOATS_PER_PADDLE_VERTEX == 2");

    float vertices[FLOATS_PER_PADDLE_VERTEX * 4];
    getPaddleVertices(vertices, paddle);

    glBindBuffer(GL_ARRAY_BUFFER, paddleVB);
    glBufferSubData(GL_ARRAY_BUFFER, 0, PADDLE_VERTICES_SIZE, vertices);
}

static void updateBallVB(const Ball* ball, unsigned int ballVB)
{
    static_assert(FLOATS_PER_BALL_VERTEX == 2, "Expected FLOATS_PER_BALL_VERTEX == 2");

    float vertices[FLOATS_PER_BALL_VERTEX * 4];
    getBallVertices(vertices, ball);

    glBindBuffer(GL_ARRAY_BUFFER, ballVB);
    glBufferSubData(GL_ARRAY_BUFFER, 0, BALL_VERTICES_SIZE, vertices);
}

void updateRenderData(GameRenderData* renderData, const GameObjects* gameObjects)
{
    updatePaddleVB(&gameObjects->paddle, renderData->paddleRenderer.VB);
    updateBallVB(&gameObjects->ball, renderData->ballRenderer.VB);
}

static void freeGameShaders(const GameShaders* shaders)
{
    glDeleteProgram(shaders->paddleShader);
    glDeleteProgram(shaders->blockShader);
    glDeleteProgram(shaders->ballShader);
}

void freeRenderData(const GameRenderData* renderData)
{
    freeGameShaders(&renderData->shaders);

    freeQuadRenderer(&renderData->paddleRenderer);
    freeInstancedQuadRenderer(&renderData->blocksRenderer);
    freeQuadRenderer(&renderData->ballRenderer);

    glDeleteBuffers(1, &renderData->quadIB);
}

static void updateBallShaderUnifs(const BallShaderUnifs* unifs, const Ball* ball)
{
    glUniform2f(unifs->normalBallCenter, normalizeCoordinate(ball->position.x),
        normalizeCoordinate(ball->position.y));
    glUniform1f(unifs->normalBallRadiusSquared, powf(normalizeLength(ball->radius), 2.0f));
}

static void drawBall(const Ball* ball, unsigned int ballShader, const BallShaderUnifs* unifs,
    unsigned int ballVA)
{
    glUseProgram(ballShader);
    updateBallShaderUnifs(unifs, ball);
    drawElements(ballVA, 6, GL_UNSIGNED_SHORT);
}

static void drawPaddle(unsigned int shader, unsigned int paddleVA)
{
    glUseProgram(shader);
    drawElements(paddleVA, 6, GL_UNSIGNED_SHORT);
}

static void drawBlocks(size_t blockCount, unsigned int shader, unsigned int blocksVA)
{
    glUseProgram(shader);
    drawInstances(blocksVA, 6, (GLsizei)blockCount, GL_UNSIGNED_SHORT);
}

void render(const GameRenderData* renderData, const GameObjects* gameObjects)
{
    drawPaddle(renderData->shaders.paddleShader, renderData->paddleRenderer.VA);
    drawBlocks(gameObjects->blockCount, renderData->shaders.blockShader, renderData->blocksRenderer.VA);
    drawBall(&gameObjects->ball, renderData->shaders.ballShader, &renderData->shaders.ballShaderUnifs,
        renderData->ballRenderer.VA);
}
