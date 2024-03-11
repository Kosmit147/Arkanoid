#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>

#define COORDINATE_SPACE 1024

typedef struct Vec2
{
    float x, y;
} Vec2;

typedef struct Vec2ui
{
    float x, y;
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

void paddleMove(Block* paddle, GLFWwindow* window, float deltaTime){
    
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        paddle->position.x += 500.0f * deltaTime;
    
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        paddle->position.x -= 500.0f * deltaTime;
}

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
    glShaderSource(vertexShader, 1, &vertexShaderSrc, NULL);
    glShaderSource(fragmentShader, 1, &fragmentShaderSrc, NULL);
    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);

    unsigned int shader = glCreateProgram();
    glAttachShader(shader, vertexShader);
    glAttachShader(shader, fragmentShader);
    glLinkProgram(shader);
    glUseProgram(shader);

    Vec2ui position = { COORDINATE_SPACE / 2, COORDINATE_SPACE / 6 };
    Block* paddle = createBlock(position, 150, 50, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, NULL);
    glEnableVertexAttribArray(0);

    float previousTime = (float) glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {

        glClear(GL_COLOR_BUFFER_BIT);
        float time = (float)glfwGetTime();
        float deltaTime = time - previousTime;

        paddleMove(paddle, window, deltaTime);

        updateBlockVB(paddle);
        drawBlock(paddle);

        glfwSwapBuffers(window);
        glfwPollEvents();
        previousTime = time;
        
    }

    free(paddle);

    glfwTerminate();
    return 0;
}