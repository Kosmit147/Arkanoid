#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>

#define INCBIN_PREFIX 
#include <incbin.h>

#include "window.h"
#include "entities.h"
#include "board.h"
#include "rendering.h"
#include "shader.h"

#include "defines.h"

// INCTXT(blockVertexShaderSrc, "../shaders/block.vert");
// INCTXT(blockFragmentShaderSrc, "../shaders/block.frag");

INCTXT(paddleVertexShaderSrc, "../shaders/paddle.vert");
INCTXT(paddleFragmentShaderSrc, "../shaders/paddle.frag");

void movePaddle(Block* paddle, unsigned int paddleVB, GLFWwindow* window, float deltaTime)
{    
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        paddle->position.x += PADDLE_SPEED * deltaTime;
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        paddle->position.x -= PADDLE_SPEED * deltaTime;

    updateBlockVB(paddle, paddleVB);
}

int main()
{
    GLFWwindow* window = setUpWindow("Arkanoid", 1200, 1200);

    if (!window)
        return -1;

    if (!loadGlad())
        return -1;

    glfwSetFramebufferSizeCallback(window, onWindowResize);
    puts((const char*)glGetString(GL_VERSION));

    unsigned int paddleVA = genVA();

    Vec2 paddlePosition = { (float)COORDINATE_SPACE / 2 - PADDLE_WIDTH / 2, (float)COORDINATE_SPACE / 6 };
    Block paddle = {
        paddlePosition,
        PADDLE_WIDTH,
        PADDLE_HEIGHT,
    };

    unsigned int paddleVB = createBlockVB(&paddle, GL_DYNAMIC_DRAW);
    unsigned int paddleIB = createBlockIB(GL_STATIC_DRAW);
    setBlockVertexAttributes();

    // unsigned int blockShader = createShader(blockVertexShaderSrcData, blockFragmentShaderSrcData);
    // glUseProgram(blockShader);

    unsigned int paddleShader = createShader(paddleVertexShaderSrcData, paddleFragmentShaderSrcData);
    glUseProgram(paddleShader);

    float prevTime = (float)glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        float time = (float)glfwGetTime();
        float deltaTime = time - prevTime;

        glClear(GL_COLOR_BUFFER_BIT);

        movePaddle(&paddle, paddleVB, window, deltaTime);
        drawVertices(paddleVA, 6, GL_UNSIGNED_SHORT);

        glfwSwapBuffers(window);
        glfwPollEvents();

        prevTime = time;
    }

    glDeleteVertexArrays(1, &paddleVA);
    glDeleteBuffers(1, &paddleVB);
    glDeleteBuffers(1, &paddleIB);
    // glDeleteProgram(blockShader);
    glDeleteProgram(paddleShader);

    glfwTerminate();
    return 0;
}