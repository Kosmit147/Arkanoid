#pragma once

#include <glad/glad.h>

#include <stddef.h>

#include "entities.h"
#include "board.h"

#include "defines.h"

typedef struct GLBuffers
{
    // after adding new buffers update freeGLBuffers()
    unsigned int VA, VB, IB;
} GLBuffers;

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

typedef struct RenderingData
{
    // after adding new data update freeRenderingData()
    GameShaders shaders;
    BallShaderUnifs ballShaderUnifs;
    GLBuffers paddleBuffers;
    GLBuffers blocksBuffers;
    GLBuffers ballBuffers;
    size_t blocksToRender;
} RenderingData;

#ifdef _DEBUG
void GLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
    GLsizei length, const GLchar* message, const void* userParam);
#endif

// Generate GL Buffers
unsigned int genVA();
unsigned int genVB();
unsigned int genIB();

// Operate on GL Buffers
void moveDataWithinGLBuffer(GLenum bufferType, unsigned int buffer, GLintptr dstOffset,
    GLintptr srcOffset, GLsizeiptr size);
void moveObjectsWithinGLBuffer(GLenum bufferType, unsigned int buffer, size_t dstIndex,
    size_t srcIndex, size_t count, size_t objSize);

// Create GLBuffers
GLBuffers createBlockGLBuffers(const Block* block);
GLBuffers createNormalizedBlocksGLBuffers(const Block* blocks, size_t blockCount);
GLBuffers createBallGLBuffers(const Ball* ball);

// Free GLBuffers
void freeGLBuffers(const GLBuffers* buffers);

// Get Vertex Data
void getBlockVertices(float* vertices, const Block* block);
void getNormalizedBlockVertices(float* vertices, const Block* block);
void getBallVertices(float* vertices, const Ball* ball);

// Create Vertex Buffers
unsigned int createBlockVB(const Block* block, GLenum usage);
unsigned int createNormalizedBlockVB(const Block* block, GLenum usage);
unsigned int createNormalizedBlocksVB(const Block* blocks, size_t count, GLenum usage);
unsigned int createBallVB(const Ball* ball, GLenum usage);

// Update Vertex Buffers
void updateBlockVB(const Block* block, unsigned int blockVB);
void updateBlocksVBOnBlocksDestroyed(unsigned int blocksVB, size_t destroyedIndex,
    size_t destroyedCount, size_t newBlockCount);
void updateBallVB(const Ball* ball, unsigned int ballVB);

// Create Index Buffers
unsigned int createBlockIB(GLenum usage);
unsigned int createBlocksIB(size_t count, GLenum usage);

// Set Vertex Attributes
void setBlockVertexAttributes();
void setBallVertexAttributes();

// Retrieve Uniforms
int retrieveUniformLocation(unsigned int shader, const char* name);
BallShaderUnifs retrieveBallShaderUnifs(unsigned int ballShader);

// Update uniforms
void updateBallUnifs(const BallShaderUnifs* unifs, const Ball* ball);

// Get Rendering Data
GameShaders createGameShaders();
RenderingData createRenderingData(const GameObjects* gameObjects);

// Update Rendering Data
void updateRenderingData(RenderingData* renderingData, const GameObjects* gameObjects);

// Free Rendering Data
void freeGameShaders(const GameShaders* shaders);
void freeRenderingData(const RenderingData* renderingData);

// Draw Vertices
void drawVertices(unsigned int VA, int count, GLenum IBType);

// Draw Game Objects
void drawBall(unsigned int shader, unsigned int ballVA);
void drawPaddle(unsigned int shader, unsigned int paddleVA);
void drawBlocks(size_t blockCount, unsigned int shader, unsigned int blocksVA);

// Render
void render(const RenderingData* renderingData);