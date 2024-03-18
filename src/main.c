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

#include "defines.h"

INCTXT(blockVertexShaderSrc, "../shaders/block.vert");
INCTXT(blockFragmentShaderSrc, "../shaders/block.frag");

INCTXT(paddleVertexShaderSrc, "../shaders/paddle.vert");
INCTXT(paddleFragmentShaderSrc, "../shaders/paddle.frag");

int main()
{
    GLFWwindow* window = setUpWindow("Arkanoid", WINDOW_WIDTH, WINDOW_HEIGHT);

    if (!window)
        return -1;

    if (!loadGlad())
        return -1;

    glfwSetFramebufferSizeCallback(window, onWindowResize);

#ifdef _DEBUG
    glDebugMessageCallback(GLDebugCallback, NULL);
    puts((const char*)glGetString(GL_VERSION));
#endif

    Block paddle = createPaddle(PADDLE_START_POS_X, PADDLE_START_POS_Y, PADDLE_WIDTH, PADDLE_HEIGHT);
    GLBuffers paddleBuffers = createBlockGLBuffers(&paddle);

    size_t blockCount;
    Block* blocks = createBlocks(1, &blockCount);
    GLBuffers blocksBuffers = createNormalizedBlocksGLBuffers(blocks, blockCount);

    unsigned int paddleShader = createShader(paddleVertexShaderSrcData, paddleFragmentShaderSrcData);
    unsigned int blockShader = createShader(blockVertexShaderSrcData, blockFragmentShaderSrcData);

    float prevTime = (float)glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        float time = (float)glfwGetTime();
        float deltaTime = time - prevTime;

        glClear(GL_COLOR_BUFFER_BIT);

        movePaddle(&paddle, window, deltaTime);
        updateBlockVB(&paddle, paddleBuffers.VB);

        glUseProgram(paddleShader);
        drawVertices(paddleBuffers.VA, 6, GL_UNSIGNED_SHORT);

        glUseProgram(blockShader);
        drawVertices(blocksBuffers.VA, (int)blockCount * 6, GL_UNSIGNED_SHORT);

        glfwSwapBuffers(window);
        glfwPollEvents();

        prevTime = time;
    }

    freeGLBuffers(&paddleBuffers);
    freeGLBuffers(&blocksBuffers);

    glDeleteProgram(paddleShader);
    glDeleteProgram(blockShader);

    free(blocks);

    glfwTerminate();
    return 0;
}