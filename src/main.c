#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define INCBIN_PREFIX 
#include <incbin.h>

#define COORDINATE_SPACE 1024

typedef struct Vec2
{
    float x, y;
} Vec2;

typedef struct Vec2ui
{
    unsigned int x, y;
} Vec2ui;

typedef struct Ball
{
    Vec2 position; // ball center
    Vec2 translation;

    unsigned int glVB; // vertex buffer
} Ball;

typedef struct Block
{
    Vec2ui position; // top-left corner
    unsigned int width;
    unsigned int height;

    unsigned int glVB; // vertex buffer
} Block;

Block* createBlock(Vec2ui position, unsigned int width, unsigned int height, GLenum usage)
{
    Block* block = malloc(sizeof(Block));

    block->position = position;
    block->width = width;
    block->height = height;

    float normalizedX = (float)position.x / COORDINATE_SPACE * 2.0f - 1.0f;
    float normalizedY = (float)position.y / COORDINATE_SPACE * 2.0f - 1.0f;
    float normalizedWidth = (float)width / COORDINATE_SPACE * 2.0f;
    float normalizedHeight = (float)height / COORDINATE_SPACE * 2.0f;

    float positions[] = {
        normalizedX, normalizedY,
        normalizedX + normalizedWidth, normalizedY,
        normalizedX + normalizedWidth, normalizedY - normalizedHeight,
        normalizedX, normalizedY - normalizedHeight,
    };

    glGenBuffers(1, &block->glVB);
    glBindBuffer(GL_ARRAY_BUFFER, block->glVB);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * 2, positions, usage);

    return block;
}

void drawBlock(Block* block)
{
    glBindBuffer(GL_ARRAY_BUFFER, block->glVB);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, NULL);
}

void updateBlockVB(Block* block)
{
    float normalizedX = (float)block->position.x / COORDINATE_SPACE * 2.0f - 1.0f;
    float normalizedY = (float)block->position.y / COORDINATE_SPACE * 2.0f - 1.0f;
    float normalizedWidth = (float)block->width / COORDINATE_SPACE * 2.0f;
    float normalizedHeight = (float)block->height / COORDINATE_SPACE * 2.0f;

    float newPositions[] = {
        normalizedX, normalizedY,
        normalizedX + normalizedWidth, normalizedY,
        normalizedX + normalizedWidth, normalizedY - normalizedHeight,
        normalizedX, normalizedY - normalizedHeight,
    };

    glBindBuffer(GL_ARRAY_BUFFER, block->glVB);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 4 * 2, newPositions);
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

    const unsigned char* glVersion = glGetString(GL_VERSION);
    puts((const char*)glVersion);

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
    const char* vertexDataPtr = vertexShaderSrcData;
    const char* fragmentDataPtr = fragmentShaderSrcData;
    glShaderSource(vertexShader, 1, &vertexDataPtr, NULL);
    glShaderSource(fragmentShader, 1, &fragmentDataPtr, NULL);
    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);

    unsigned int shader = glCreateProgram();
    glAttachShader(shader, vertexShader);
    glAttachShader(shader, fragmentShader);
    glLinkProgram(shader);
    glUseProgram(shader);

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

    free(block);

    glfwTerminate();
    return 0;
}