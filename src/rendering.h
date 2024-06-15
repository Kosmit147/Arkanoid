/// @file rendering.h
/// @brief Functionality related to rendering game objects and UI.

#pragma once

#include "gl.h"
#include "game_state.h"
#include "board.h"

/// @brief Struct for paddle shader uniforms.
typedef struct PaddleShaderUnifs
{
    GLint borderRectTopLeft;
    GLint borderRectBottomRight;
    GLint color;
} PaddleShaderUnifs;

/// @brief Struct for ball shader uniforms.
typedef struct BallShaderUnifs
{
    GLint ballCenter;
    GLint ballRadiusSquared;
    GLint color;
} BallShaderUnifs;

/// @brief Struct for game shaders.
typedef struct GameShaders
{
    GLuint paddleShader;
    PaddleShaderUnifs paddleShaderUnifs;
    GLuint blockShader;
    GLuint ballShader;
    BallShaderUnifs ballShaderUnifs;
} GameShaders;

/// @brief Struct for game renderer.
typedef struct GameRenderer
{
    GameShaders shaders;
    QuadRenderer paddleRenderer;
    InstancedQuadRenderer blocksRenderer;
    QuadRenderer ballRenderer;
} GameRenderer;

/// @brief Struct for HUD shaders.
typedef struct HudShaders
{
    GLuint textRendererShader;

#ifdef DRAW_QUAD_TREE
    GLuint debugShader;
#endif
} HudShaders;

/// @brief Struct for HUD renderer.
typedef struct HudRenderer
{
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

/// @brief Initialize the renderer.
/// @param renderer The renderer to initialize.
/// @param board The game board.
void initRenderer(Renderer* renderer, const Board* board);

/// @brief Update the renderer.
/// @param renderer The renderer to update.
/// @param board The game board.
void updateRenderer(Renderer* renderer, const Board* board);

/// @brief Free the resources used by the renderer.
/// @param renderer The renderer to free.
void freeRenderer(const Renderer* renderer);

/// @brief Render the game and HUD.
/// @param renderer The renderer to use.
/// @param state The current game state.
/// @param board The game board.
void render(const Renderer* renderer, const GameState* state, const Board* board);

/// @brief Initialize the game renderer.
/// @param renderer The game renderer to initialize.
/// @param board The game board.
/// @param quadIB The index buffer object for quads.
void initGameRenderer(GameRenderer* renderer, const Board* board, GLuint quadIB);

/// @brief Update the game renderer.
/// @param renderer The game renderer to update.
/// @param board The game board.
void updateGameRenderer(GameRenderer* renderer, const Board* board);

/// @brief Move a block out of view.
/// @param renderer The game renderer.
/// @param blockIndex The index of the block to move.
void moveBlockOutOfView(GameRenderer* renderer, size_t blockIndex);

/// @brief Free the resources used by the game renderer.
/// @param renderer The game renderer to free.
void freeGameRenderer(const GameRenderer* renderer);

/// @brief Render the game.
/// @param renderer The game renderer.
/// @param board The game board.
void renderGame(const GameRenderer* renderer, const Board* board);

/// @brief Initialize the HUD renderer.
/// @param renderer The HUD renderer to initialize.
/// @param board The game board.
/// @param quadIB The index buffer object for quads.
void initHudRenderer(HudRenderer* renderer, const Board* board, GLuint quadIB);

/// @brief Update the HUD level text.
/// @param renderer The HUD renderer.
/// @param newLevel The new level to display.
void updateHudLevelText(HudRenderer* renderer, unsigned int newLevel);

/// @brief Update the HUD points text.
/// @param renderer The HUD renderer.
/// @param newPoints The new points to display.
void updateHudPointsText(HudRenderer* renderer, unsigned int newPoints);

#ifdef DRAW_QUAD_TREE
void redrawQuadTree(HudRenderer* renderer, const QuadTree* quadTree);
#endif

/// @brief Free the resources used by the HUD renderer.
/// @param renderer The HUD renderer to free.
void freeHudRenderer(const HudRenderer* renderer);

/// @brief Render the HUD.
/// @param renderer The HUD renderer.
/// @param state The current game state.
void renderHud(const HudRenderer* renderer, const GameState* state);
