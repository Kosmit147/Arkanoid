#pragma once

#include "gl.h"
#include "board.h"

typedef struct PaddleShaderUnifs
{
    GLint color;
} PaddleShaderUnifs;

typedef struct BallShaderUnifs
{
    GLint normalizedBallCenter;
    GLint normalizedBallRadiusSquared;
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
} HudShaders;

typedef struct HudRenderer
{
    // after adding new data update freeHudRenderer()
    HudShaders shaders;
    bool drawGameOverText;
    TextRenderer gameOverRenderer;
    TextRenderer pressRestartGameKeyRenderer;
    TextRenderer pointsRenderer;
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
void render(const Renderer* renderer, const Board* board);

void initGameRenderer(GameRenderer* renderer, const Board* board, GLuint quadIB);
void updateGameRenderer(GameRenderer* renderer, const Board* board);
void freeGameRenderer(const GameRenderer* renderer);
void renderGame(const GameRenderer* renderer, const Board* board);

void initHudRenderer(HudRenderer* renderer, GLuint quadIB);
void freeHudRenderer(const HudRenderer* renderer);
void renderHud(const HudRenderer* renderer);
