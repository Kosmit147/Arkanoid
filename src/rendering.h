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
} GameShaders;

typedef struct GameRenderer
{
    // after adding new data update freeGameRenderer()
    GameShaders shaders;
    QuadRenderer paddleRenderer;
    InstancedQuadRenderer blocksRenderer;
    QuadRenderer ballRenderer;
} GameRenderer;

typedef struct HudShaders
{
    // after adding new data update freeHudShaders()
    GLuint textRendererShader;

#ifdef DRAW_QUAD_TREE
    GLuint debugShader;
#endif
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

#ifdef DRAW_QUAD_TREE
    LineRenderer quadTreeRenderer;
#endif
} HudRenderer;

typedef struct Renderer
{
    GameRenderer gameRenderer;
    HudRenderer hudRenderer;
    GLuint quadIB;
} Renderer;

// TODO: clean up (make static)
void initRenderer(Renderer* renderer, const Board* board);
void updateRenderer(Renderer* renderer, const Board* board);
void freeRenderer(const Renderer* renderer);
void render(const Renderer* renderer, const GameState* state, const Board* board);

void initGameRenderer(GameRenderer* renderer, const Board* board, GLuint quadIB);
void updateGameRenderer(GameRenderer* renderer, const Board* board);
void moveBlockOutOfView(GameRenderer* renderer, size_t blockIndex);
void freeGameRenderer(const GameRenderer* renderer);
void renderGame(const GameRenderer* renderer, const Board* board);

void initHudRenderer(HudRenderer* renderer, const Board* board, GLuint quadIB);
void updateHudLevelText(HudRenderer* renderer, unsigned int newLevel);
void updateHudPointsText(HudRenderer* renderer, unsigned int newPoints);
#ifdef DRAW_QUAD_TREE
void redrawQuadTree(HudRenderer* renderer, const QuadTree* quadTree);
#endif
void freeHudRenderer(const HudRenderer* renderer);
void renderHud(const HudRenderer* renderer, const GameState* state);
