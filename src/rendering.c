#include "rendering.h"

#include <glad/glad.h>
#include <stb_image.h>

#define INCBIN_PREFIX
#include <incbin.h>

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

#include "shader.h"
#include "texture.h"

#include "defines.h"

INCTXT(commonShaderSrc, "../shaders/common.glsl");
INCTXT(blockVertexShaderSrc, "../shaders/block.vert");
INCTXT(blockFragmentShaderSrc, "../shaders/block.frag");
INCTXT(paddleVertexShaderSrc, "../shaders/paddle.vert");
INCTXT(paddleFragmentShaderSrc, "../shaders/paddle.frag");
INCTXT(ballVertexShaderSrc, "../shaders/ball.vert");
INCTXT(ballFragmentShaderSrc, "../shaders/ball.frag");

INCTXT(textRendererVertexShaderSrc, "../shaders/text.vert");
INCTXT(textRendererFragmentShaderSrc, "../shaders/text.frag");

INCBIN(fontTexture, "../res/font.png");

void initRenderer(Renderer* renderer, const Board* board)
{
    renderer->quadIB = createQuadIB(MAX_QUADS, GL_STATIC_DRAW);
    initGameRenderer(&renderer->gameRenderer, board, renderer->quadIB);
    initHudRenderer(&renderer->hudRenderer, renderer->quadIB);
}

void updateRenderer(Renderer* renderer, const Board* board)
{
    updateGameRenderer(&renderer->gameRenderer, board);
}

void freeRenderer(const Renderer* renderer)
{
    freeGameRenderer(&renderer->gameRenderer);
    freeHudRenderer(&renderer->hudRenderer);
    glDeleteBuffers(1, &renderer->quadIB);
}

void render(const Renderer* renderer, const Board* board)
{
    renderGame(&renderer->gameRenderer, board);
    renderHud(&renderer->hudRenderer);
}

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

    RectBounds borderBounds = getBlockBorderBounds(block);
    vertices[2] = normalizeCoordinate(borderBounds.topLeft.x);
    vertices[3] = normalizeCoordinate(borderBounds.topLeft.y);
    vertices[4] = normalizeCoordinate(borderBounds.bottomRight.x);
    vertices[5] = normalizeCoordinate(borderBounds.bottomRight.y);

    Vec4 color = getRandomBlockColor();
    vertices[6] = color.r;
    vertices[7] = color.g;
    vertices[8] = color.b;
    vertices[9] = color.a;
}

static void createBlocksInstanceBufferImpl(const QuadTree* quadTree, float** vertices)
{
    for (size_t i = 0; i < MAX_OBJECTS; i++)
    {
        if (quadTree->objects[i] != NULL)
        {
            const Block* block = quadTree->objects[i];
            getBlockInstanceVertices(*vertices, block);
            *vertices += FLOATS_PER_BLOCK_INSTANCE_VERTEX;
        }
    }

    for (size_t i = 0; i < 4; i++)
    {
        if (quadTree->nodes[i] != NULL)
        {
            createBlocksInstanceBufferImpl(quadTree->nodes[i], vertices);
        }
    }
}

// TODO: this shouldn't be public
unsigned int createBlocksInstanceBuffer(const QuadTree* quadTree)
{
    unsigned int instBuff = genVB();

    float* vertices = malloc(BLOCK_INSTANCE_VERTICES_SIZE * quadTree->objCount);
    float* currentVerticesAddr = vertices;

    createBlocksInstanceBufferImpl(quadTree, &currentVerticesAddr);

    glBufferData(GL_ARRAY_BUFFER, (GLsizei)BLOCK_INSTANCE_VERTICES_SIZE * (GLsizei)quadTree->objCount, vertices,
        GL_DYNAMIC_DRAW);

    free(vertices);

    return instBuff;
}

// static unsigned int createBlocksInstanceBuffer(const Block* blocks, size_t blockCount)
// {
//     unsigned int instBuff = genVB();
// 
//     float* vertices = malloc(BLOCK_INSTANCE_VERTICES_SIZE * blockCount);
// 
//     for (size_t i = 0; i < blockCount; i++)
//         getBlockInstanceVertices(vertices + i * FLOATS_PER_BLOCK_INSTANCE_VERTEX, &blocks[i]);
// 
//     glBufferData(GL_ARRAY_BUFFER, (GLsizei)BLOCK_INSTANCE_VERTICES_SIZE * (GLsizei)blockCount, vertices,
//         GL_DYNAMIC_DRAW);
// 
//     free(vertices);
// 
//     return instBuff;
// }

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
        .normalizedBallCenter = retrieveUniformLocation(ballShader, "normalizedBallCenter"),
        .normalizedBallRadiusSquared = retrieveUniformLocation(ballShader, "normalizedBallRadiusSquared"),
        .color = retrieveUniformLocation(ballShader, "color"),
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
    QuadRenderer renderer = {
        .VA = genVA(),
        .VB = createPaddleVB(paddle),
    };

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIB);
    setPaddleRendererVertexAttributes();

    return renderer;
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

static InstancedQuadRenderer createBlocksRenderer(const QuadTree* quadTree, unsigned int quadIB)
{
    Block baseBlock = {
        // start at (-1.0, -1.0), use a translation vector in the shader
        .position = (Vec2){ .x = -1.0f, .y = -1.0f },
        .width = 0.0f,
        .height = 0.0f,
    };

    // assumes all blocks have the same width and height
    if (quadTree->objCount > 0)
    {
        const Block* block = retrieveNth(quadTree, 0);
        baseBlock.width = block->width;
        baseBlock.height = block->height;
    }

    InstancedQuadRenderer renderer = {
        .VA = genVA(),
        .VB = createBlockVB(&baseBlock),
        .instanceBuffer = createBlocksInstanceBuffer(quadTree),
    };

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIB);
    setBlockRendererVertexAttributes(renderer.VB, renderer.instanceBuffer);

    return renderer;
}

// static InstancedQuadRenderer createBlocksRenderer(const Block* blocks, size_t blockCount,
//     unsigned int quadIB)
// {
//     Block baseBlock = {
//         // start at (-1.0, -1.0), use a translation vector in the shader
//         .position = (Vec2){ .x = -1.0f, .y = -1.0f },
//         .width = 0.0f,
//         .height = 0.0f,
//     };
// 
//     // assumes all blocks have the same width and height
//     if (blockCount > 0)
//     {
//         baseBlock.width = blocks[0].width;
//         baseBlock.height = blocks[0].height;
//     }
// 
//     InstancedQuadRenderer renderer = {
//         .VA = genVA(),
//         .VB = createBlockVB(&baseBlock),
//         .instanceBuffer = createBlocksInstanceBuffer(blocks, blockCount),
//     };
// 
//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIB);
//     setBlockRendererVertexAttributes(renderer.VB, renderer.instanceBuffer);
// 
//     return renderer;
// }

static void setBallRendererVertexAttributes()
{
    static_assert(FLOATS_PER_BALL_VERTEX == 2, "Expected FLOATS_PER_BALL_VERTEX == 2");

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * FLOATS_PER_BALL_VERTEX, NULL);
    glEnableVertexAttribArray(0);
}

static QuadRenderer createBallRenderer(const Ball* ball, unsigned int quadIB)
{
    QuadRenderer renderer = {
        .VA = genVA(),
        .VB = createBallVB(ball),
    };

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIB);
    setBallRendererVertexAttributes();

    return renderer;
}

static void initPaddleShaderUnifs(const PaddleShaderUnifs* unifs, const Vec4* paddleColor)
{
    glUniform4f(unifs->color, paddleColor->r, paddleColor->g, paddleColor->b, paddleColor->a);
}

static void initBallShaderUnifs(const BallShaderUnifs* unifs, const Vec4* ballColor)
{
    glUniform4f(unifs->color, ballColor->r, ballColor->g, ballColor->b, ballColor->a);
}

void initGameRenderer(GameRenderer* renderer, const Board* board, unsigned int quadIB)
{
    renderer->shaders = createGameShaders();

    glUseProgram(renderer->shaders.paddleShader);
    Vec4 paddleColor = getPaddleColor();
    initPaddleShaderUnifs(&renderer->shaders.paddleShaderUnifs, &paddleColor);

    glUseProgram(renderer->shaders.ballShader);
    Vec4 ballColor = getBallColor();
    initBallShaderUnifs(&renderer->shaders.ballShaderUnifs, &ballColor);

    renderer->paddleRenderer = createPaddleRenderer(&board->paddle, quadIB);
    renderer->blocksRenderer = createBlocksRenderer(board->quadTree, quadIB);
    renderer->ballRenderer = createBallRenderer(&board->ball, quadIB);
}

static void updatePaddleVB(const Block* paddle, unsigned int paddleVB)
{
    float vertices[FLOATS_PER_PADDLE_VERTEX * 4];
    getPaddleVertices(vertices, paddle);

    glBindBuffer(GL_ARRAY_BUFFER, paddleVB);
    glBufferSubData(GL_ARRAY_BUFFER, 0, PADDLE_VERTICES_SIZE, vertices);
}

static void updateBallVB(const Ball* ball, unsigned int ballVB)
{
    float vertices[FLOATS_PER_BALL_VERTEX * 4];
    getBallVertices(vertices, ball);

    glBindBuffer(GL_ARRAY_BUFFER, ballVB);
    glBufferSubData(GL_ARRAY_BUFFER, 0, BALL_VERTICES_SIZE, vertices);
}

void updateGameRenderer(GameRenderer* renderer, const Board* board)
{
    updatePaddleVB(&board->paddle, renderer->paddleRenderer.VB);
    updateBallVB(&board->ball, renderer->ballRenderer.VB);
}

static void freeGameShaders(const GameShaders* shaders)
{
    glDeleteProgram(shaders->paddleShader);
    glDeleteProgram(shaders->blockShader);
    glDeleteProgram(shaders->ballShader);
}

void freeGameRenderer(const GameRenderer* renderer)
{
    freeGameShaders(&renderer->shaders);

    freeQuadRenderer(&renderer->paddleRenderer);
    freeInstancedQuadRenderer(&renderer->blocksRenderer);
    freeQuadRenderer(&renderer->ballRenderer);
}

static void updateBallShaderUnifs(const BallShaderUnifs* unifs, const Ball* ball)
{
    glUniform2f(unifs->normalizedBallCenter, normalizeCoordinate(ball->position.x),
        normalizeCoordinate(ball->position.y));
    glUniform1f(unifs->normalizedBallRadiusSquared, powf(normalizeLength(ball->radius), 2.0f));
}

static void drawBall(const Ball* ball, unsigned int shader, const BallShaderUnifs* unifs,
    unsigned int ballVA)
{
    glUseProgram(shader);
    updateBallShaderUnifs(unifs, ball);
    drawElements(ballVA, 6, QUAD_IB_DATA_TYPE);
}

static void drawPaddle(unsigned int shader, unsigned int paddleVA)
{
    glUseProgram(shader);
    drawElements(paddleVA, 6, QUAD_IB_DATA_TYPE);
}

static void drawBlocks(size_t blockCount, unsigned int shader, unsigned int blocksVA)
{
    glUseProgram(shader);
    drawInstances(blocksVA, 6, (GLsizei)blockCount, QUAD_IB_DATA_TYPE);
}

void renderGame(const GameRenderer* renderer, const Board* board)
{
    drawPaddle(renderer->shaders.paddleShader, renderer->paddleRenderer.VA);
    drawBlocks(board->quadTree->objCount, renderer->shaders.blockShader, renderer->blocksRenderer.VA);
    drawBall(&board->ball, renderer->shaders.ballShader, &renderer->shaders.ballShaderUnifs,
        renderer->ballRenderer.VA);
}

static HudShaders createHudShaders()
{
    return (HudShaders) {
        .textRendererShader = createShader(textRendererVertexShaderSrcData,
            textRendererFragmentShaderSrcData, ARKANOID_GL_SHADER_VERSION_DECL),
    };
}

static unsigned int createHudRendererFontTexture()
{
    int texWidth;
    int texHeight;
    int texChannels;

    unsigned char* texData = stbi_load_from_memory(fontTextureData, (int)fontTextureSize, &texWidth,
        &texHeight, &texChannels, 1);

    GLint swizzleMask[4] = { GL_RED, GL_RED, GL_RED, GL_RED };
    TextureOptions texOptions = {
        .horizontalWrap = GL_REPEAT,
        .verticalWrap = GL_REPEAT,
        .minFilter = GL_LINEAR,
        .magFilter = GL_LINEAR,
        .borderColor = NULL,
        .swizzleMask = swizzleMask,
    };

    unsigned int texture = createTexture(GL_TEXTURE0, texWidth, texHeight, texData, GL_UNSIGNED_BYTE, GL_RED,
        GL_RED, &texOptions);

    stbi_image_free(texData);
    return texture;
}

void initHudRenderer(HudRenderer* renderer, unsigned int quadIB)
{
    renderer->shaders = createHudShaders();

    unsigned int texture = createHudRendererFontTexture();

    renderer->font = (BitmapFont) {
        .cols = 8,
        .rows = 12,
        .offset = ' ',
        .textureID = texture,
    };

    Vec2 gameOverPos = {
        .x = 0.0f - FONT_WIDTH * (float)strlen(GAME_OVER_STR) / 2.0f,
        .y = 0.0f + FONT_HEIGHT,
    };

    Vec2 pressRestartGameKeyPos = {
        .x = 0.0f - FONT_WIDTH * (float)strlen(PRESS_RESTART_GAME_KEY_STR) / 2.0f,
        .y = 0.0f,
    };

    Vec2 pointsPos = { .x = -1.0f, .y = -1.0f + FONT_HEIGHT};

    renderer->drawGameOverText = false;
    renderer->gameOverRenderer = createTextRenderer(GAME_OVER_STR, strlen(GAME_OVER_STR), &renderer->font,
        gameOverPos, FONT_WIDTH, FONT_HEIGHT, quadIB);
    renderer->pressRestartGameKeyRenderer = createTextRenderer(PRESS_RESTART_GAME_KEY_STR,
        strlen(PRESS_RESTART_GAME_KEY_STR), &renderer->font, pressRestartGameKeyPos, FONT_WIDTH, FONT_HEIGHT,
        quadIB);
    renderer->pointsRenderer = createTextRenderer(POINTS_STR, strlen(POINTS_STR), &renderer->font, pointsPos,
        FONT_WIDTH, FONT_HEIGHT, quadIB);
}

static void freeHudShaders(const HudShaders* shaders)
{
    glDeleteProgram(shaders->textRendererShader);
}

void freeHudRenderer(const HudRenderer* renderer)
{
    freeHudShaders(&renderer->shaders);

    freeBitmapFont(&renderer->font);
    freeTextRenderer(&renderer->gameOverRenderer);
    freeTextRenderer(&renderer->pressRestartGameKeyRenderer);
    freeTextRenderer(&renderer->pointsRenderer);
}

void renderHud(const HudRenderer* renderer)
{
    glUseProgram(renderer->shaders.textRendererShader);

    if (renderer->drawGameOverText)
    {
        renderText(&renderer->gameOverRenderer);
        renderText(&renderer->pressRestartGameKeyRenderer);
    }

    renderText(&renderer->pointsRenderer);
}
