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

typedef struct GameRenderingData
{
    // after adding new data update freeRenderingData()
    GameShaders shaders;
    BallShaderUnifs ballShaderUnifs;
    unsigned int quadIB;
    GLQuad paddleQuad;
    GLInstancedQuad blocksQuad;
    GLQuad ballQuad;
} GameRenderingData;

void initRenderingData(GameRenderingData* data, const GameObjects* gameObjects);
void updateRenderingData(GameRenderingData* renderingData, const GameObjects* gameObjects);
void freeRenderingData(const GameRenderingData* renderingData);
void render(const GameRenderingData* renderingData, const GameObjects* gameObjects);
