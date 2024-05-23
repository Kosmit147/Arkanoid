#pragma once

#include "gl.h"
#include "game_state.h"
#include "board.h"

typedef struct PaddleShaderUnifs
{
    GLint borderRectTopLeft;
    GLint borderRectBottomRight;
    GLint color;
} PaddleShaderUnifs;

typedef struct BallShaderUnifs
{
    GLint ballCenter;
    GLint ballRadiusSquared;
    GLint color;
} BallShaderUnifs;

typedef struct GameShaders
{
    // after adding new shaders update freeGameShaders()
    GLuint paddleShader;
    PaddleShaderUnifs paddleShaderUnifs;
    GLuint blockShader;
    GLuint ballShader;
    BallShaderUnifs ballShaderUnifs;

#ifdef DRAW_QUAD_TREE
    GLuint debugShader;
#endif
} GameShaders;

typedef struct GameRenderer
{
    // after adding new data update freeGameRenderer()
    GameShaders shaders;
    QuadRenderer paddleRenderer;
    InstancedQuadRenderer blocksRenderer;
    QuadRenderer ballRenderer;

#ifdef DRAW_QUAD_TREE
    size_t quadTreeNodeCount;
    QuadRenderer quadTreeRenderer;
#endif
} GameRenderer;

typedef struct HudShaders
{
    // after adding new data update freeHudShaders()
    GLuint textRendererShader;
} HudShaders;

typedef struct HudRenderer
{
    // after adding new data update freeHudRenderer()
    HudShaders shaders;
    TextRenderer launchBallControlsRenderer;
    TextRenderer paddleControlsRenderer;
    TextRenderer levelRenderer;
    TextRenderer pointsRenderer;
    TextRenderer gameOverRenderer;
    TextRenderer pressRestartGameKeyRenderer;
    BitmapFont font;
} HudRenderer;

typedef struct Renderer
{
    GameRenderer gameRenderer;
    HudRenderer hudRenderer;
    GLuint quadIB;
} Renderer;

void initRenderer(Renderer* renderer, const Board* board);
void updateRenderer(Renderer* renderer, const Board* board);
void freeRenderer(const Renderer* renderer);
void render(const Renderer* renderer, const GameState* state, const Board* board);

void initGameRenderer(GameRenderer* renderer, const Board* board, GLuint quadIB);
void updateGameRenderer(GameRenderer* renderer, const Board* board);
void deleteBlockFromGameRenderer(GameRenderer* renderer, size_t index);
void freeGameRenderer(const GameRenderer* renderer);
void renderGame(const GameRenderer* renderer, const Board* board);

void initHudRenderer(HudRenderer* renderer, GLuint quadIB);
void updateHudLevelText(HudRenderer* renderer, unsigned int newLevel);
void updateHudPointsText(HudRenderer* renderer, unsigned int newPoints);
void freeHudRenderer(const HudRenderer* renderer);
void renderHud(const HudRenderer* renderer, const GameState* state);
