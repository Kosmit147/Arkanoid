#include "rendering.h"

#include <glad/glad.h>
#include <stb_image.h>

#define INCBIN_PREFIX
#include <incbin.h>

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#include "gl.h"
#include "quad_tree.h"
#include "str_utils.h"
#include "vector.h"
#include "shader.h"
#include "texture.h"
#include "entities.h"

#include "defines.h"

INCTXT(commonShaderSrc, SHADERS_PATH "common.glsl");
INCTXT(blockVertexShaderSrc, SHADERS_PATH "block.vert");
INCTXT(blockFragmentShaderSrc, SHADERS_PATH "block.frag");
INCTXT(paddleVertexShaderSrc, SHADERS_PATH "paddle.vert");
INCTXT(paddleFragmentShaderSrc, SHADERS_PATH "paddle.frag");
INCTXT(ballVertexShaderSrc, SHADERS_PATH "ball.vert");
INCTXT(ballFragmentShaderSrc, SHADERS_PATH "ball.frag");

#ifdef DRAW_QUAD_TREE
INCTXT(debugVertexShaderSrc, SHADERS_PATH "debug.vert");
INCTXT(debugFragmentShaderSrc, SHADERS_PATH "debug.frag");
#endif

INCTXT(textRendererVertexShaderSrc, SHADERS_PATH "text.vert");
INCTXT(textRendererFragmentShaderSrc, SHADERS_PATH "text.frag");

INCBIN(fontTexture, RES_PATH "font.png");

// After modifying vertex structs remember to update the appropriate
// getVertices and setVertexAttributes functions
typedef struct PaddleVertex
{
    Vec2 position;
} PaddleVertex;

typedef struct BlockVertex
{
    Vec2 position;
} BlockVertex;

typedef struct BlockInstanceVertex
{
    Vec2 translation;
    RectBounds borderRect;
    Vec4 color;
} BlockInstanceVertex;

typedef struct BallVertex
{
    Vec2 position;
} BallVertex;

void initRenderer(Renderer* renderer, const Board* board)
{
    renderer->quadIB = createQuadIB(MAX_QUADS, GL_STATIC_DRAW);
    initGameRenderer(&renderer->gameRenderer, board, renderer->quadIB);
    initHudRenderer(&renderer->hudRenderer, board, renderer->quadIB);
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

void render(const Renderer* renderer, const GameState* state, const Board* board)
{
    renderGame(&renderer->gameRenderer, board);
    renderHud(&renderer->hudRenderer, state);
}

static void getPaddleVertices(PaddleVertex vertices[4], const Block* paddle)
{
    float x1 = paddle->position.x;
    float x2 = paddle->position.x + paddle->width;
    float y1 = paddle->position.y;
    float y2 = paddle->position.y - paddle->height;

    vertices[0] = (PaddleVertex) { .position = { .x = x1, .y = y1 } };
    vertices[1] = (PaddleVertex) { .position = { .x = x2, .y = y1 } };
    vertices[2] = (PaddleVertex) { .position = { .x = x2, .y = y2 } };
    vertices[3] = (PaddleVertex) { .position = { .x = x1, .y = y2 } };
}

static GLuint createPaddleVB(const Block* paddle)
{
    GLuint VB = genVB();

    PaddleVertex vertices[4];
    getPaddleVertices(vertices, paddle);

    glBufferData(GL_ARRAY_BUFFER, sizeof(PaddleVertex) * 4, vertices, GL_DYNAMIC_DRAW);

    return VB;
}

static void getBlockVertices(BlockVertex vertices[4], const Block* block)
{
    float normalizedX1 = block->position.x;
    float normalizedY1 = block->position.y;
    float normalizedX2 = normalizedX1 + normalizeLength(block->width);
    float normalizedY2 = normalizedY1 - normalizeLength(block->height);

    vertices[0] = (BlockVertex) { .position = { .x = normalizedX1, .y = normalizedY1 } };
    vertices[1] = (BlockVertex) { .position = { .x = normalizedX2, .y = normalizedY1 } };
    vertices[2] = (BlockVertex) { .position = { .x = normalizedX2, .y = normalizedY2 } };
    vertices[3] = (BlockVertex) { .position = { .x = normalizedX1, .y = normalizedY2 } };
}

static GLuint createBlockVB(const Block* block)
{
    GLuint VB = genVB();

    BlockVertex vertices[4];
    getBlockVertices(vertices, block);
    glBufferData(GL_ARRAY_BUFFER, sizeof(BlockVertex) * 4, vertices, GL_STATIC_DRAW);

    return VB;
}

static BlockInstanceVertex getBlockInstanceVertex(const Block* block)
{
    return (BlockInstanceVertex) {
        .translation = (Vec2) {
            .x = normalizeLength(block->position.x),
            .y = normalizeLength(block->position.y),
        },
        .borderRect = normalizeRectBounds(getBlockBorderRect(block)),
        .color = getRandomBlockColor(),
    };
}

static unsigned int createBlocksInstanceBuffer(const Block* blocks, size_t blockCount)
{
    unsigned int instBuff = genVB();

    BlockInstanceVertex* vertices = checkedMalloc(sizeof(BlockInstanceVertex) * blockCount);

    for (size_t i = 0; i < blockCount; i++)
        vertices[i] = getBlockInstanceVertex(&blocks[i]);

    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(sizeof(BlockInstanceVertex) * blockCount), vertices,
        GL_DYNAMIC_DRAW);

    free(vertices);

    return instBuff;
}

static void getBallVertices(BallVertex vertices[4], const Ball* ball)
{
    float x1 = ball->position.x - ball->radius;
    float x2 = ball->position.x + ball->radius;
    float y1 = ball->position.y - ball->radius;
    float y2 = ball->position.y + ball->radius;

    vertices[0] = (BallVertex){ .position = { .x = x1, .y = y1 } };
    vertices[1] = (BallVertex){ .position = { .x = x2, .y = y1 } };
    vertices[2] = (BallVertex){ .position = { .x = x2, .y = y2 } };
    vertices[3] = (BallVertex){ .position = { .x = x1, .y = y2 } };
}

static GLuint createBallVB(const Ball* ball)
{
    GLuint VB = genVB();

    BallVertex vertices[4];
    getBallVertices(vertices, ball);
    glBufferData(GL_ARRAY_BUFFER, sizeof(BallVertex) * 4, vertices, GL_DYNAMIC_DRAW);

    return VB;
}

static PaddleShaderUnifs retrievePaddleShaderUnifs(GLuint paddleShader)
{
    return (PaddleShaderUnifs) {
        .borderRectTopLeft = retrieveUniformLocation(paddleShader, "borderRect.topLeft"),
        .borderRectBottomRight = retrieveUniformLocation(paddleShader, "borderRect.bottomRight"),
        .color = retrieveUniformLocation(paddleShader, "color"),
    };
}

static BallShaderUnifs retrieveBallShaderUnifs(GLuint ballShader)
{
    return (BallShaderUnifs) {
        .ballCenter = retrieveUniformLocation(ballShader, "ballCenter"),
        .ballRadiusSquared = retrieveUniformLocation(ballShader, "ballRadiusSquared"),
        .color = retrieveUniformLocation(ballShader, "color"),
    };
}

static GameShaders createGameShaders()
{
    GameShaders shaders;
    setCommonShaderSrc(commonShaderSrcData);

    shaders.paddleShader = createShader(paddleVertexShaderSrcData, paddleFragmentShaderSrcData,
        ARKANOID_GL_SHADER_VERSION_DECL);
    shaders.blockShader = createShader(blockVertexShaderSrcData, blockFragmentShaderSrcData,
        ARKANOID_GL_SHADER_VERSION_DECL);
    shaders.ballShader = createShader(ballVertexShaderSrcData, ballFragmentShaderSrcData,
        ARKANOID_GL_SHADER_VERSION_DECL);

    shaders.paddleShaderUnifs = retrievePaddleShaderUnifs(shaders.paddleShader);
    shaders.ballShaderUnifs = retrieveBallShaderUnifs(shaders.ballShader);

    return shaders;
}

static void setPaddleRendererVertexAttributes()
{
    vertexAttribfv(0, PaddleVertex, position);
}

static QuadRenderer createPaddleRenderer(const Block* paddle, GLuint quadIB)
{
    QuadRenderer renderer = {
        .VA = genVA(),
        .VB = createPaddleVB(paddle),
    };

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIB);
    setPaddleRendererVertexAttributes();

    return renderer;
}

static void setBlockRendererVertexAttributes(GLuint VB, GLuint instanceBuffer)
{
    glBindBuffer(GL_ARRAY_BUFFER, VB);
    vertexAttribfv(0, BlockVertex, position);

    glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);
    instVertexAttribfv(1, BlockInstanceVertex, translation);
    instVertexAttribfv(2, BlockInstanceVertex, borderRect.topLeft);
    instVertexAttribfv(3, BlockInstanceVertex, borderRect.bottomRight);
    instVertexAttribfv(4, BlockInstanceVertex, color);
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
    vertexAttribfv(0, BallVertex, position);
}

static QuadRenderer createBallRenderer(const Ball* ball, GLuint quadIB)
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

void initGameRenderer(GameRenderer* renderer, const Board* board, GLuint quadIB)
{
    renderer->shaders = createGameShaders();

    glUseProgram(renderer->shaders.paddleShader);
    Vec4 paddleColor = getPaddleColor();
    initPaddleShaderUnifs(&renderer->shaders.paddleShaderUnifs, &paddleColor);

    glUseProgram(renderer->shaders.ballShader);
    Vec4 ballColor = getBallColor();
    initBallShaderUnifs(&renderer->shaders.ballShaderUnifs, &ballColor);

    renderer->paddleRenderer = createPaddleRenderer(&board->paddle, quadIB);
    renderer->blocksRenderer = createBlocksRenderer(board->blocksStorage, board->originalBlockCount, quadIB);
    renderer->ballRenderer = createBallRenderer(&board->ball, quadIB);
}

static void updatePaddleRenderer(const Block* paddle, const QuadRenderer* paddleRenderer)
{
    PaddleVertex vertices[4];
    getPaddleVertices(vertices, paddle);
    glBindBuffer(GL_ARRAY_BUFFER, paddleRenderer->VB);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(PaddleVertex) * 4, vertices);
}

static void updateBallRenderer(const Ball* ball, const QuadRenderer* ballRenderer)
{
    BallVertex vertices[4];
    getBallVertices(vertices, ball);
    glBindBuffer(GL_ARRAY_BUFFER, ballRenderer->VB);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(BallVertex) * 4, vertices);
}

void updateGameRenderer(GameRenderer* renderer, const Board* board)
{
    updatePaddleRenderer(&board->paddle, &renderer->paddleRenderer);
    updateBallRenderer(&board->ball, &renderer->ballRenderer);
}

void moveBlockOutOfView(GameRenderer* renderer, size_t blockIndex)
{
    BlockInstanceVertex invalidVertex;

    invalidVertex.translation.x = COORDINATE_SPACE * 2.0f;
    invalidVertex.translation.y = -COORDINATE_SPACE * 2.0f;

    replaceObjectInGLBuffer(GL_ARRAY_BUFFER, renderer->blocksRenderer.instanceBuffer, blockIndex,
        &invalidVertex, sizeof(BlockInstanceVertex));
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
    glUniform2f(unifs->ballCenter, normalizeCoordinate(ball->position.x),
        normalizeCoordinate(ball->position.y));
    glUniform1f(unifs->ballRadiusSquared, powf(normalizeLength(ball->radius), 2.0f));
}

static void drawBall(const Ball* ball, GLuint shader, const BallShaderUnifs* unifs, GLuint ballRendererVA)
{
    glUseProgram(shader);
    updateBallShaderUnifs(unifs, ball);
    drawElements(ballRendererVA, 6, QUAD_IB_DATA_TYPE);
}

static void updatePaddleShaderUnifs(const PaddleShaderUnifs* unifs, const Block* paddle)
{
    RectBounds borderRect = getBlockBorderRect(paddle);
    glUniform2f(unifs->borderRectTopLeft, borderRect.topLeft.x, borderRect.topLeft.y);
    glUniform2f(unifs->borderRectBottomRight, borderRect.bottomRight.x, borderRect.bottomRight.y);
}

static void drawPaddle(const Block* paddle, GLuint shader, const PaddleShaderUnifs* unifs,  GLuint paddleVA)
{
    glUseProgram(shader);
    updatePaddleShaderUnifs(unifs, paddle);
    drawElements(paddleVA, 6, QUAD_IB_DATA_TYPE);
}

static void drawBlocks(size_t blockCount, GLuint shader, GLuint blocksRendererVA)
{
    glUseProgram(shader);
    drawInstances(blocksRendererVA, 6, (GLsizei)blockCount, QUAD_IB_DATA_TYPE);
}

void renderGame(const GameRenderer* renderer, const Board* board)
{
    drawBlocks(board->originalBlockCount, renderer->shaders.blockShader, renderer->blocksRenderer.VA);
    drawPaddle(&board->paddle, renderer->shaders.paddleShader, &renderer->shaders.paddleShaderUnifs,
        renderer->paddleRenderer.VA);
    drawBall(&board->ball, renderer->shaders.ballShader, &renderer->shaders.ballShaderUnifs,
        renderer->ballRenderer.VA);
}

static HudShaders createHudShaders()
{
    return (HudShaders) {
        .textRendererShader = createShader(textRendererVertexShaderSrcData, textRendererFragmentShaderSrcData,
            ARKANOID_GL_SHADER_VERSION_DECL),

#ifdef DRAW_QUAD_TREE
        .debugShader = createShader(debugVertexShaderSrcData, debugFragmentShaderSrcData,
            ARKANOID_GL_SHADER_VERSION_DECL),
#endif
    };
}

// TODO: factor out into some create texture from memory function
static GLuint createHudRendererFontTexture()
{
    int texWidth;
    int texHeight;
    int texChannels;

    stbi_uc* texData = stbi_load_from_memory(fontTextureData, (int)fontTextureSize, &texWidth, &texHeight,
        &texChannels, 1);

    GLint swizzleMask[4] = { GL_RED, GL_RED, GL_RED, GL_RED };
    TextureOptions texOptions = {
        .horizontalWrap = GL_REPEAT,
        .verticalWrap = GL_REPEAT,
        .minFilter = GL_LINEAR,
        .magFilter = GL_LINEAR,
        .borderColor = NULL,
        .swizzleMask = swizzleMask,
    };

    GLuint texture = createTexture(GL_TEXTURE0, texWidth, texHeight, texData, GL_UNSIGNED_BYTE, GL_RED,
        GL_RED, &texOptions);

    stbi_image_free(texData);
    return texture;
}

#ifdef DRAW_QUAD_TREE
static void getQuadTreeRendererPoints(const QuadTreeNode* node, Vector* result)
{
    Vec2 points[8];

    if (quadTreeNodeHasSubnodes(node))
    {
        for (size_t i = 0; i < 4; i++)
            getQuadTreeRendererPoints(node->nodes[i], result);
    }

    Vec2 topRight = { .x = node->bounds.bottomRight.x, .y = node->bounds.topLeft.y };
    Vec2 bottomLeft = { .x = node->bounds.topLeft.x, .y = node->bounds.bottomRight.y };

    points[0] = node->bounds.topLeft;       points[1] = topRight;
    points[2] = topRight;                   points[3] = node->bounds.bottomRight;
    points[4] = node->bounds.bottomRight;   points[5] = bottomLeft;
    points[6] = bottomLeft;                 points[7] = node->bounds.topLeft;

    for (size_t i = 0; i < arrLength(points); i++)
    {
        points[i] = normalizePoint(points[i]);
        vectorPushBack(result, &points[i], sizeof(Vec2));
    }
}

static LineRenderer createQuadTreeRenderer(const QuadTree* quadTree)
{
    Vector points = vectorCreate();
    vectorReserve(&points, 100, sizeof(Vec2));

    getQuadTreeRendererPoints(quadTree->root, &points);

    LineRenderer renderer = createLineRenderer(points.data, vectorSize(&points, sizeof(Vec2)),
        GL_STATIC_DRAW);

    vectorFree(&points);

    return renderer;
}
#endif

void initHudRenderer(HudRenderer* renderer, const Board* board, GLuint quadIB)
{
    renderer->shaders = createHudShaders();
    GLuint texture = createHudRendererFontTexture();

    renderer->font = (BitmapFont) {
        .cols = 8,
        .rows = 12,
        .offset = ' ',
        .textureID = texture,
    };

    static const Vec2 launchBallControlsPos = {
        .x = 0.0f - CONTROLS_SCREEN_FONT_WIDTH * (float)staticStrLen(LAUNCH_BALL_CONTROLS_STR) / 2.0f,
        .y = 0.0f - CONTROLS_SCREEN_FONT_HEIGHT * 1.5f,
    };

    static const Vec2 paddleControlsPos = {
        .x = 0.0f - CONTROLS_SCREEN_FONT_WIDTH * (float)staticStrLen(PADDLE_CONTROLS_STR) / 2.0f,
        .y = 0.0f - CONTROLS_SCREEN_FONT_HEIGHT * 2.5f,
    };

    static const Vec2 levelPos = { .x = -1.0f, .y = -1.0f + STATS_FONT_HEIGHT * 2.0f};
    static const Vec2 pointsPos = { .x = -1.0f, .y = -1.0f + STATS_FONT_HEIGHT};

    static const Vec2 gameOverPos = {
        .x = 0.0f - GAME_OVER_SCREEN_FONT_WIDTH * (float)staticStrLen(GAME_OVER_STR) / 2.0f,
        .y = 0.0f + GAME_OVER_SCREEN_FONT_HEIGHT,
    };

    static const Vec2 pressRestartGameKeyPos = {
        .x = 0.0f - GAME_OVER_SCREEN_FONT_WIDTH * (float)staticStrLen(PRESS_RESTART_GAME_KEY_STR) / 2.0f,
        .y = 0.0f,
    };

    renderer->launchBallControlsRenderer = createTextRenderer(LAUNCH_BALL_CONTROLS_STR,
        staticStrLen(LAUNCH_BALL_CONTROLS_STR), &renderer->font, launchBallControlsPos,
        CONTROLS_SCREEN_FONT_WIDTH, CONTROLS_SCREEN_FONT_HEIGHT, quadIB);

    renderer->paddleControlsRenderer = createTextRenderer(PADDLE_CONTROLS_STR,
        staticStrLen(PADDLE_CONTROLS_STR), &renderer->font, paddleControlsPos, CONTROLS_SCREEN_FONT_WIDTH,
        CONTROLS_SCREEN_FONT_HEIGHT, quadIB);

    renderer->levelRenderer = createTextRenderer(LEVEL_FIRST_STR, staticStrLen(LEVEL_FIRST_STR),
        &renderer->font, levelPos, STATS_FONT_WIDTH, STATS_FONT_HEIGHT, quadIB);

    renderer->pointsRenderer = createTextRenderer(POINTS_0_STR, staticStrLen(POINTS_0_STR), &renderer->font,
        pointsPos, STATS_FONT_WIDTH, STATS_FONT_HEIGHT, quadIB);

    renderer->gameOverRenderer = createTextRenderer(GAME_OVER_STR, staticStrLen(GAME_OVER_STR),
        &renderer->font, gameOverPos, GAME_OVER_SCREEN_FONT_WIDTH, GAME_OVER_SCREEN_FONT_HEIGHT, quadIB);

    renderer->pressRestartGameKeyRenderer = createTextRenderer(PRESS_RESTART_GAME_KEY_STR,
        staticStrLen(PRESS_RESTART_GAME_KEY_STR), &renderer->font, pressRestartGameKeyPos,
        GAME_OVER_SCREEN_FONT_WIDTH, GAME_OVER_SCREEN_FONT_HEIGHT, quadIB);

#ifdef DRAW_QUAD_TREE
    renderer->quadTreeRenderer = createQuadTreeRenderer(&board->blocksQuadTree);
#else
    (void)(board); // board is unused when compiling without DRAW_QUAD_TREE
#endif
}

void updateHudLevelText(HudRenderer* renderer, unsigned int newLevel)
{
    char levelText[staticStrLen(LEVEL_STR) + MAX_DIGITS_IN_LEVEL_NUM + 1] = LEVEL_STR;
    size_t charsWritten = uiToStr(newLevel, levelText + staticStrLen(LEVEL_STR),
        MAX_DIGITS_IN_LEVEL_NUM + 1);

    updateTextRenderer(&renderer->levelRenderer, levelText, staticStrLen(LEVEL_STR) + charsWritten,
        renderer->levelRenderer.position);
}

void updateHudPointsText(HudRenderer* renderer, unsigned int newPoints)
{
    char pointsText[staticStrLen(POINTS_STR) + MAX_DIGITS_IN_POINTS_NUM + 1] = POINTS_STR;
    size_t charsWritten = uiToStr(newPoints, pointsText + staticStrLen(POINTS_STR),
        MAX_DIGITS_IN_POINTS_NUM + 1);

    updateTextRenderer(&renderer->pointsRenderer, pointsText, staticStrLen(POINTS_STR) + charsWritten,
        renderer->pointsRenderer.position);
}

static void freeHudShaders(const HudShaders* shaders)
{
    glDeleteProgram(shaders->textRendererShader);

#ifdef DRAW_QUAD_TREE
    glDeleteProgram(shaders->debugShader);
#endif
}

#ifdef DRAW_QUAD_TREE
void redrawQuadTree(HudRenderer* renderer, const QuadTree* quadTree)
{
    freeLineRenderer(&renderer->quadTreeRenderer);
    renderer->quadTreeRenderer = createQuadTreeRenderer(quadTree);
}
#endif

void freeHudRenderer(const HudRenderer* renderer)
{
    freeHudShaders(&renderer->shaders);
    freeBitmapFont(&renderer->font);

    freeTextRenderer(&renderer->launchBallControlsRenderer);
    freeTextRenderer(&renderer->paddleControlsRenderer);
    freeTextRenderer(&renderer->levelRenderer);
    freeTextRenderer(&renderer->pointsRenderer);
    freeTextRenderer(&renderer->gameOverRenderer);
    freeTextRenderer(&renderer->pressRestartGameKeyRenderer);

#ifdef DRAW_QUAD_TREE
    freeLineRenderer(&renderer->quadTreeRenderer);
#endif
}

void renderHud(const HudRenderer* renderer, const GameState* state)
{
    glUseProgram(renderer->shaders.textRendererShader);

    if (!state->gameStarted)
    {
        renderText(&renderer->paddleControlsRenderer);
        renderText(&renderer->launchBallControlsRenderer);
    }

    if (state->gameOver)
    {
        renderText(&renderer->gameOverRenderer);
        renderText(&renderer->pressRestartGameKeyRenderer);
    }

    renderText(&renderer->levelRenderer);
    renderText(&renderer->pointsRenderer);

#ifdef DRAW_QUAD_TREE
    glUseProgram(renderer->shaders.debugShader);
    renderLines(&renderer->quadTreeRenderer);
#endif
}
