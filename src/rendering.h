#pragma once

#include "gl.h"
#include "board.h"

typedef struct BallShaderUnifs
{
    int normalBallCenter;
    int normalBallRadiusSquared;
} BallShaderUnifs;

typedef struct GameShaders
{
    // after adding new shaders update freeGameShaders()
    unsigned int paddleShader;
    unsigned int blockShader;
    unsigned int ballShader;
} GameShaders;

typedef struct GameRenderData
{
    // after adding new data update freeRenderingData()
    GameShaders shaders;
    BallShaderUnifs ballShaderUnifs;
    unsigned int quadIB;
    GLQuad paddleQuad;
    GLInstancedQuad blocksQuad;
    GLQuad ballQuad;
} GameRenderData;

void initRenderData(GameRenderData* data, const GameObjects* gameObjects);
void updateRenderData(GameRenderData* renderData, const GameObjects* gameObjects);
void freeRenderData(const GameRenderData* renderData);
void render(const GameRenderData* renderData, const GameObjects* gameObjects);
