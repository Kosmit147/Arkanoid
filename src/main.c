#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define INCBIN_PREFIX
#include <incbin.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "window.h"
#include "entities.h"
#include "board.h"
#include "rendering.h"
#include "shader.h"
#include "input.h"
#include "helpers.h"

#include "defines.h"

INCTXT(blockVertexShaderSrc, "../shaders/block.vert");
INCTXT(blockFragmentShaderSrc, "../shaders/block.frag");
INCTXT(paddleVertexShaderSrc, "../shaders/paddle.vert");
INCTXT(paddleFragmentShaderSrc, "../shaders/paddle.frag");
INCTXT(ballVertexShaderSrc, "../shaders/ball.vert");
INCTXT(ballFragmentShaderSrc, "../shaders/ball.frag");

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

    Ball ball = createBall(BALL_START_POS_X, BALL_START_POS_Y, BALL_RADIUS, BALL_TRANSLATION_X, BALL_TRANSLATION_Y);
    GLBuffers ballBuffers = createBallGLBuffers(&ball);

    size_t blockCount;
    Block* blocks = createBlocks(STARTING_LEVEL, &blockCount);
    GLBuffers blocksBuffers = createNormalizedBlocksGLBuffers(blocks, blockCount);

    unsigned int paddleShader = createShader(paddleVertexShaderSrcData,
        paddleFragmentShaderSrcData, GL_SHADER_VERSION_DECL);
    unsigned int blockShader = createShader(blockVertexShaderSrcData,
        blockFragmentShaderSrcData, GL_SHADER_VERSION_DECL);
    unsigned int ballShader = createShader(ballVertexShaderSrcData,
        ballFragmentShaderSrcData, GL_SHADER_VERSION_DECL);

    int ballCenterUnifLocation = glGetUniformLocation(ballShader, "normalBallCenter");
    glUniform2f(ballCenterUnifLocation, normalizeCoordinate(ball.position.x), normalizeCoordinate(ball.position.y));

    int ballRadiusSquaredUnifLocation = glGetUniformLocation(ballShader, "normalBallRadiusSquared");
    glUniform1f(ballRadiusSquaredUnifLocation, (float)pow(normalizeLength(ball.radius), 2));

    float prevTime = (float)glfwGetTime();

    removeBlock(blocks, &blockCount, 1);
    updateBlocksVBOnBlockDestroyed(blocksBuffers.VB, 1, blockCount);

    while (!glfwWindowShouldClose(window))
    {
        float time = (float)glfwGetTime();
        float deltaTime = minf(time - prevTime, DELTA_TIME_LIMIT);

        glClear(GL_COLOR_BUFFER_BIT);

        movePaddle(&paddle, window, deltaTime);
        updateBlockVB(&paddle, paddleBuffers.VB);

        glUseProgram(paddleShader);
        drawVertices(paddleBuffers.VA, 6, GL_UNSIGNED_SHORT);

        glUseProgram(ballShader);
        drawVertices(ballBuffers.VA, 6, GL_UNSIGNED_SHORT);

        glUseProgram(blockShader);
        drawVertices(blocksBuffers.VA, (int)blockCount * 6, GL_UNSIGNED_SHORT);

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