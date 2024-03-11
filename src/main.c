#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "entities.h"
#include "board.h"
#include "rendering.h"

#include "defines.h"

const char* vertexShaderSrc = 
"#version 430 core\n"
"\n"
"layout (location = 0) in vec4 inPosition;\n"
"\n"
"void main()\n"
"{\n"
"   gl_Position = inPosition;\n"
"}\n";
const char* fragmentShaderSrc = 
"#version 430 core\n"
"\n"
"out vec4 outColor;\n"
"\n"
"void main()\n"
"{\n"
"   outColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
"}\n";

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

    unsigned int vertexShader;
    unsigned int fragmentShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSrc, NULL);
    glShaderSource(fragmentShader, 1, &fragmentShaderSrc, NULL);
    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);

    unsigned int shader = glCreateProgram();
    glAttachShader(shader, vertexShader);
    glAttachShader(shader, fragmentShader);
    glLinkProgram(shader);
    glUseProgram(shader);

#ifndef _DEBUG
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
#endif

    Vec2ui position = { COORDINATE_SPACE / 2, COORDINATE_SPACE / 2 };
    Block* block = createBlock(position, 100, 100, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, NULL);
    glEnableVertexAttribArray(0);

    unsigned int centerX = COORDINATE_SPACE / 2;
    unsigned int centerY = COORDINATE_SPACE / 2;

    while (!glfwWindowShouldClose(window))
    {
        float time = (float)glfwGetTime();

        glClear(GL_COLOR_BUFFER_BIT);

        block->position.x = centerX + (unsigned int)(sin(time) * 500.0f);
        block->position.y = centerY + (unsigned int)(cos(time) * 500.0f);

        updateBlockVB(block);
        drawBlock(block);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &blockVA);
    glDeleteBuffers(1, &block->glVB);
    glDeleteBuffers(1, &blockIB);
    glDeleteShader(shader);

    free(block);

    glfwTerminate();
    return 0;
}