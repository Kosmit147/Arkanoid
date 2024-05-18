#include "rendering.h"

#include <glad/glad.h>
#include <stb_image.h>

#define INCBIN_PREFIX
#include <incbin.h>

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

#include "gl.h"
#include "helpers.h"
#include "vector.h"
#include "memory.h"
#include "shader.h"
#include "texture.h"
#include "entities.h"

#include "defines.h"

INCTXT(commonShaderSrc, "../shaders/common.glsl");
INCTXT(blockVertexShaderSrc, "../shaders/block.vert");
INCTXT(blockFragmentShaderSrc, "../shaders/block.frag");
INCTXT(paddleVertexShaderSrc, "../shaders/paddle.vert");
INCTXT(paddleFragmentShaderSrc, "../shaders/paddle.frag");
INCTXT(ballVertexShaderSrc, "../shaders/ball.vert");
INCTXT(ballFragmentShaderSrc, "../shaders/ball.frag");

#ifdef DRAW_QUAD_TREE
INCTXT(debugVertexShaderSrc, "../shaders/debug.vert");
INCTXT(debugFragmentShaderSrc, "../shaders/debug.frag");
#endif

INCTXT(textRendererVertexShaderSrc, "../shaders/text.vert");
INCTXT(textRendererFragmentShaderSrc, "../shaders/text.frag");

INCBIN(fontTexture, "../res/font.png");

// Vertex structs must be packed in order to correctly send data to OpenGL
// After modifying remember to update the appropriate getVertices and setVertexAttributes functions
ENSURE_PACKED

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

#ifdef DRAW_QUAD_TREE
typedef struct QuadTreeNodeVertex
{
    Vec2 position;
} QuadTreeNodeVertex;
#endif

END_ENSURE_PACKED

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

static void createBlocksInstanceBufferImpl(const QuadTree* quadTree, BlockInstanceVertex** vertices)
{
    for (size_t i = 0; i < MAX_OBJECTS; i++)
    {
        if (quadTree->objects[i] != NULL)
        {
            const Block* block = quadTree->objects[i];
            **vertices = getBlockInstanceVertex(block);
            (*vertices)++;
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
GLuint createBlocksInstanceBuffer(const QuadTree* quadTree)
{
    GLuint instBuff = genVB();

    BlockInstanceVertex* vertices = checkedMalloc(sizeof(BlockInstanceVertex) * quadTree->objCount);
    BlockInstanceVertex* currAddr = vertices;

    createBlocksInstanceBufferImpl(quadTree, &currAddr);

    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(sizeof(BlockInstanceVertex) * quadTree->objCount), vertices,
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

#ifdef DRAW_QUAD_TREE
static void getQuadTreeNodeVertices(QuadTreeNodeVertex vertices[4], const QuadTree* quadTree) 
{
    float x1 = normalizeCoordinate(quadTree->bounds.position.x);
    float x2 = normalizeCoordinate(quadTree->bounds.position.x + quadTree->bounds.width);
    float y1 = normalizeCoordinate(quadTree->bounds.position.y);
    float y2 = normalizeCoordinate(quadTree->bounds.position.y - quadTree->bounds.height);

    vertices[0] = (QuadTreeNodeVertex){ .position = { .x = x1, .y = y1 }, };
    vertices[1] = (QuadTreeNodeVertex){ .position = { .x = x2, .y = y1 }, };
    vertices[2] = (QuadTreeNodeVertex){ .position = { .x = x2, .y = y2 }, };
    vertices[3] = (QuadTreeNodeVertex){ .position = { .x = x1, .y = y2 }, };
}

static void createQuadTreeVBImpl(const QuadTree* quadTree, Vector* vertices)
{
    QuadTreeNodeVertex tmp[4];
    getQuadTreeNodeVertices(tmp, quadTree);

    vectorPushBack(vertices, &tmp[0], QuadTreeNodeVertex);
    vectorPushBack(vertices, &tmp[1], QuadTreeNodeVertex);
    vectorPushBack(vertices, &tmp[2], QuadTreeNodeVertex);
    vectorPushBack(vertices, &tmp[3], QuadTreeNodeVertex);

    for (size_t i = 0; i < 4; i++)
    {
        if (quadTree->nodes[i] != NULL)
        {
            createQuadTreeVBImpl(quadTree->nodes[i], vertices);
        }
    }
}

static GLuint createQuadTreeVB(const QuadTree* quadTree, size_t* quadTreeNodeCount)
{
    GLuint VB = genVB();

    Vector vertices = vectorCreate();

    createQuadTreeVBImpl(quadTree, &vertices);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)vertices.size, vertices.data, GL_DYNAMIC_DRAW);

    *quadTreeNodeCount = vectorSize(&vertices, QuadTreeNodeVertex) / 4;

    vectorFree(&vertices);
    return VB;
}
#endif

static PaddleShaderUnifs retrievePaddleShaderUnifs(GLuint paddleShader)
{
    return (PaddleShaderUnifs) {
        .color = retrieveUniformLocation(paddleShader, "color"),
    };
}

static BallShaderUnifs retrieveBallShaderUnifs(GLuint ballShader)
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
        
#ifdef DRAW_QUAD_TREE
    shaders.transparentShader = createShader(debugVertexShaderSrcData,
        debugFragmentShaderSrcData, ARKANOID_GL_SHADER_VERSION_DECL),
#endif

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

static InstancedQuadRenderer createBlocksRenderer(const QuadTree* quadTree, unsigned int quadIB)
{
    // TODO: fix this hacky solution
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

#ifdef DRAW_QUAD_TREE
static void setQuadTreeRendererVertexAttributes()
{
    vertexAttribfv(0, QuadTreeNodeVertex, position);
}

static QuadRenderer createQuadTreeRenderer(const QuadTree* quadTree, GLuint quadIB, size_t* quadTreeNodeCount)
{
    QuadRenderer renderer = {
        .VA = genVA(),
        .VB = createQuadTreeVB(quadTree, quadTreeNodeCount),
    };

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIB);
    setQuadTreeRendererVertexAttributes();

    return renderer;
}
#endif

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
    renderer->blocksRenderer = createBlocksRenderer(board->quadTree, quadIB);
    renderer->ballRenderer = createBallRenderer(&board->ball, quadIB);
#ifdef DRAW_QUAD_TREE
    renderer->quadTreeRenderer = createQuadTreeRenderer(board->quadTree, quadIB, &renderer->quadTreeNodeCount);
#endif
}

static void updatePaddleVB(const Block* paddle, GLuint paddleVB)
{
    PaddleVertex vertices[4];
    getPaddleVertices(vertices, paddle);
    glBindBuffer(GL_ARRAY_BUFFER, paddleVB);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(PaddleVertex) * 4, vertices);
}

static void updateBallVB(const Ball* ball, GLuint ballVB)
{
    BallVertex vertices[4];
    getBallVertices(vertices, ball);
    glBindBuffer(GL_ARRAY_BUFFER, ballVB);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(BallVertex) * 4, vertices);
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
    
#ifdef DRAW_QUAD_TREE
    glDeleteProgram(shaders->transparentShader);
#endif
}

void freeGameRenderer(const GameRenderer* renderer)
{
    freeGameShaders(&renderer->shaders);

    freeQuadRenderer(&renderer->paddleRenderer);
    freeInstancedQuadRenderer(&renderer->blocksRenderer);
    freeQuadRenderer(&renderer->ballRenderer);
#ifdef DRAW_QUAD_TREE
    freeQuadRenderer(&renderer->quadTreeRenderer);
#endif
}

static void updateBallShaderUnifs(const BallShaderUnifs* unifs, const Ball* ball)
{
    glUniform2f(unifs->normalizedBallCenter, normalizeCoordinate(ball->position.x),
        normalizeCoordinate(ball->position.y));
    glUniform1f(unifs->normalizedBallRadiusSquared, powf(normalizeLength(ball->radius), 2.0f));
}

static void drawBall(const Ball* ball, GLuint shader, const BallShaderUnifs* unifs, GLuint ballRendererVA)
{
    glUseProgram(shader);
    updateBallShaderUnifs(unifs, ball);
    drawElements(ballRendererVA, 6, QUAD_IB_DATA_TYPE);
}

static void drawPaddle(GLuint shader, GLuint paddleRendererVA)
{
    glUseProgram(shader);
    drawElements(paddleRendererVA, 6, QUAD_IB_DATA_TYPE);
}

static void drawBlocks(size_t blockCount, GLuint shader, GLuint blocksRendererVA)
{
    glUseProgram(shader);
    drawInstances(blocksRendererVA, 6, (GLsizei)blockCount, QUAD_IB_DATA_TYPE);
}

#ifdef DRAW_QUAD_TREE
static void drawQuadTree(size_t nodeCount, GLuint shader, GLuint quadTreeRendererVA)
{
    glUseProgram(shader);
    drawElements(quadTreeRendererVA, (GLsizei)(nodeCount * 6), QUAD_IB_DATA_TYPE);
}
#endif

void renderGame(const GameRenderer* renderer, const Board* board)
{
    drawPaddle(renderer->shaders.paddleShader, renderer->paddleRenderer.VA);
    drawBlocks(board->quadTree->objCount, renderer->shaders.blockShader, renderer->blocksRenderer.VA);
    drawBall(&board->ball, renderer->shaders.ballShader, &renderer->shaders.ballShaderUnifs,
        renderer->ballRenderer.VA);
    
#ifdef DRAW_QUAD_TREE
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(2.5f);
    drawQuadTree(renderer->quadTreeNodeCount, renderer->shaders.transparentShader,
        renderer->quadTreeRenderer.VA);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
}

static HudShaders createHudShaders()
{
    return (HudShaders) {
        .textRendererShader = createShader(textRendererVertexShaderSrcData,
            textRendererFragmentShaderSrcData, ARKANOID_GL_SHADER_VERSION_DECL),
    };
}

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

void initHudRenderer(HudRenderer* renderer, GLuint quadIB)
{
    renderer->shaders = createHudShaders();

    GLuint texture = createHudRendererFontTexture();

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
