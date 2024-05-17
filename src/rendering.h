#pragma once

#include "gl.h"
#include "board.h"

typedef struct PaddleShaderUnifs
{
    int color;
} PaddleShaderUnifs;

typedef struct BallShaderUnifs
{
    int normalBallCenter;
    int normalBallRadiusSquared;
    int color;
} BallShaderUnifs;

typedef struct GameShaders
{
    // after adding new shaders update freeGameShaders()
    unsigned int paddleShader;
    PaddleShaderUnifs paddleShaderUnifs;
    unsigned int blockShader;
    unsigned int ballShader;
    BallShaderUnifs ballShaderUnifs;
} GameShaders;

typedef struct GameRenderData
{
    // after adding new data update freeRenderingData()
    GameShaders shaders;
    unsigned int quadIB;
    QuadRenderer paddleRenderer;
    InstancedQuadRenderer blocksRenderer;
    QuadRenderer ballRenderer;
} GameRenderData;

// TODO: this shouldn't be public
unsigned int createBlocksInstanceBuffer(const Quadtree* quadTree);

void initRenderData(GameRenderData* data, const GameObjects* gameObjects);
void updateRenderData(GameRenderData* renderData, const GameObjects* gameObjects);
void freeRenderData(const GameRenderData* renderData);
void render(const GameRenderData* renderData, const GameObjects* gameObjects);
