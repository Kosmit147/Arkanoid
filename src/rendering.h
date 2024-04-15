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
} RenderingData;

#ifdef _DEBUG
void GLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
    GLsizei length, const GLchar* message, const void* userParam);
#endif

unsigned int genVA();
unsigned int genVB();
unsigned int genIB();

int retrieveUniformLocation(unsigned int shader, const char* name);
void drawVertices(unsigned int VA, int count, GLenum IBType);

void moveDataWithinGLBuffer(GLenum bufferType, unsigned int buffer, GLintptr dstOffset,
    GLintptr srcOffset, GLsizeiptr size);
void moveObjectsWithinGLBuffer(GLenum bufferType, unsigned int buffer, size_t dstIndex,
    size_t srcIndex, size_t count, size_t objSize);
void eraseObjectFromGLBuffer(GLenum bufferType, unsigned int buffer, size_t index,
    size_t objectCount, size_t objSize);

void freeGLBuffers(const GLBuffers* buffers);

void initRenderingData(RenderingData* data, const GameObjects* gameObjects);
void updateRenderingData(RenderingData* renderingData, const GameObjects* gameObjects);
void freeRenderingData(const RenderingData* renderingData);
void render(const RenderingData* renderingData, const GameObjects* gameObjects);
