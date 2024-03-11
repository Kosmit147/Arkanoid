#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>

#define INCBIN_PREFIX 
#include <incbin.h>

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
    GLFWwindow* window;

    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW.\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(1200, 1200, "Arkanoid", NULL, NULL);

    if (!window)
    {
        fprintf(stderr, "Failed to create a window.\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        fprintf(stderr, "Failed to initialize GLAD.\n");
        glfwTerminate();
        return -1;
    }

    puts((const char*)glGetString(GL_VERSION));

    unsigned int blockVA;
    glGenVertexArrays(1, &blockVA);
    glBindVertexArray(blockVA);

    unsigned char indices[] = {
        0, 1, 2,
        0, 2, 3,
    };

    unsigned int blockIB;
    glGenBuffers(1, &blockIB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, blockIB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned char) * 2 * 3, indices, GL_STATIC_DRAW);

    unsigned int blockShader = createShader(vertexShaderSrcData, fragmentShaderSrcData);
    glUseProgram(blockShader);

    Vec2 position = { COORDINATE_SPACE / 2, COORDINATE_SPACE / 6 };
    Block* paddle = createBlock(position, 600, 200, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, NULL);
    glEnableVertexAttribArray(0);

    float prevTime = (float)glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        float time = (float)glfwGetTime();
        float deltaTime = time - prevTime;

        glClear(GL_COLOR_BUFFER_BIT);

        movePaddle(paddle, window, deltaTime);

        updateBlockVB(paddle);
        drawBlock(paddle);

        glfwSwapBuffers(window);
        glfwPollEvents();

        prevTime = time;
    }

    glDeleteVertexArrays(1, &blockVA);
    glDeleteBuffers(1, &paddle->glVB);
    glDeleteBuffers(1, &blockIB);
    glDeleteProgram(blockShader);

    free(paddle);

    glfwTerminate();
    return 0;
}