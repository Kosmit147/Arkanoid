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

void movePaddle(Block* paddle, GLFWwindow* window, float deltaTime)
{    
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        paddle->position.x += 2000.0f * deltaTime;
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        paddle->position.x -= 2000.0f * deltaTime;
}

INCTXT(vertexShaderSrc, "../shaders/block.vert");
INCTXT(fragmentShaderSrc, "../shaders/block.frag");

int main()
{
    GLFWwindow* window = setUpWindow("Arkanoid", 1200, 1200);

    if (!window)
        return -1;

    if (!loadGlad())
        return -1;

    puts((const char*)glGetString(GL_VERSION));

    unsigned int VA = genVA();
    unsigned int rectangleIB = genRectangleIB(GL_STATIC_DRAW);

    unsigned int blockShader = createShader(vertexShaderSrcData, fragmentShaderSrcData);
    glUseProgram(blockShader);

    const unsigned int paddleWidth = 600;
    const unsigned int paddleHeight = 200;

    Vec2 position = { (float)COORDINATE_SPACE / 2 - (float)paddleWidth / 2, (float)COORDINATE_SPACE / 6 };
    Block* paddle = createBlock(position, paddleWidth, paddleHeight, GL_DYNAMIC_DRAW);

    float prevTime = (float)glfwGetTime();

    int timeUnifLocation = glGetUniformLocation(blockShader, "time");

    while (!glfwWindowShouldClose(window))
    {
        float time = (float)glfwGetTime();
        float deltaTime = time - prevTime;

        glUniform1f(timeUnifLocation, time);

        glClear(GL_COLOR_BUFFER_BIT);

        movePaddle(paddle, window, deltaTime);

        updateBlockVB(paddle);
        drawBlock(paddle);

        glfwSwapBuffers(window);
        glfwPollEvents();

        prevTime = time;
    }

    glDeleteVertexArrays(1, &VA);
    glDeleteBuffers(1, &paddle->glVB);
    glDeleteBuffers(1, &rectangleIB);
    glDeleteProgram(blockShader);

    free(paddle);

    glfwTerminate();
    return 0;
}