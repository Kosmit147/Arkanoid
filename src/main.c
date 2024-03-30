#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define INCBIN_PREFIX
#include <incbin.h>

#include <stdio.h>
#include <stdlib.h>

#include "window.h"
#include "entities.h"
#include "board.h"
#include "rendering.h"
#include "shader.h"
#include "input.h"
#include "helpers.h"

#include "defines.h"

INCTXT(commonShaderSrc, "../shaders/common.glsl");
INCTXT(blockVertexShaderSrc, "../shaders/block.vert");
INCTXT(blockFragmentShaderSrc, "../shaders/block.frag");
INCTXT(paddleVertexShaderSrc, "../shaders/paddle.vert");
INCTXT(paddleFragmentShaderSrc, "../shaders/paddle.frag");
INCTXT(ballVertexShaderSrc, "../shaders/ball.vert");
INCTXT(ballFragmentShaderSrc, "../shaders/ball.frag");

float time;
float deltaTime;

int main()
{
    GLFWwindow* window = setUpWindow("Arkanoid", WINDOW_WIDTH, WINDOW_HEIGHT);

    if (!window)
        return -1;

    if (!loadGlad())
        return -1;

    glfwSetFramebufferSizeCallback(window, onWindowResize);
    resetWindowViewport(window);

#ifdef _DEBUG
    glDebugMessageCallback(GLDebugCallback, NULL);
    puts((const char*)glGetString(GL_VERSION));
#endif

    Block paddle = createPaddle(PADDLE_START_POS_X, PADDLE_START_POS_Y, PADDLE_WIDTH, PADDLE_HEIGHT);
    GLBuffers paddleBuffers = createBlockGLBuffers(&paddle);

    Ball ball = createBall(BALL_START_POS_X, BALL_START_POS_Y, BALL_RADIUS, 0.0f, 0.0f);
    GLBuffers ballBuffers = createBallGLBuffers(&ball);

    size_t blockCount;
    Block* const blocks = createBlocks(STARTING_LEVEL, &blockCount);
    GLBuffers blocksBuffers = createNormalizedBlocksGLBuffers(blocks, blockCount);

    setExtraShaderSrc(commonShaderSrcData);

    unsigned int paddleShader = createShader(paddleVertexShaderSrcData,
        paddleFragmentShaderSrcData, ARKANOID_GL_SHADER_VERSION_DECL);
    unsigned int blockShader = createShader(blockVertexShaderSrcData,
        blockFragmentShaderSrcData, ARKANOID_GL_SHADER_VERSION_DECL);
    unsigned int ballShader = createShader(ballVertexShaderSrcData,
        ballFragmentShaderSrcData, ARKANOID_GL_SHADER_VERSION_DECL);

    BallShaderUnifs ballShaderUnifs = retrieveBallShaderUnifs(ballShader);

    removeBlock(blocks, &blockCount, 1);
    updateBlocksVBOnBlockDestroyed(blocksBuffers.VB, 1, blockCount);

    float prevTime = (float)glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        time = (float)glfwGetTime();
        deltaTime = minf(time - prevTime, DELTA_TIME_LIMIT);

        glClear(GL_COLOR_BUFFER_BIT);

        movePaddle(&paddle, window);
        moveBall(&ball, &paddle, window);

        updateBlockVB(&paddle, paddleBuffers.VB);
        updateBallVB(&ball, ballBuffers.VB);

        drawPaddle(&paddle, paddleShader, paddleBuffers.VA);
        drawBlocks(blocks, blockCount, blockShader, blocksBuffers.VA);
        drawBall(&ball, ballShader, &ballShaderUnifs, ballBuffers.VA);

        glfwSwapBuffers(window);
        glfwPollEvents();

        prevTime = time;
    }

    freeGLBuffers(&paddleBuffers);
    freeGLBuffers(&blocksBuffers);
    freeGLBuffers(&ballBuffers);

    glDeleteProgram(paddleShader);
    glDeleteProgram(blockShader);
    glDeleteProgram(ballShader);

    free(blocks);

    glfwTerminate();
    return 0;
}