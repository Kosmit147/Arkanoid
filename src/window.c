#include "window.h"

#include <stdio.h>

GLFWwindow* setUpWindow(const char* title, int width, int height)
{
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW.\n");
        return NULL;
    }

    GLFWwindow* window;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef _DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(width, height, title, NULL, NULL);

    if (!window)
    {
        fprintf(stderr, "Failed to create a window.\n");
        glfwTerminate();
        return NULL;
    }

    glfwSetWindowAspectRatio(window, 1, 1);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    return window;
}

bool loadGlad()
{
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        fprintf(stderr, "Failed to initialize GLAD.\n");
        glfwTerminate();
        return false;
    }

    return true;
}

void onWindowResize(GLFWwindow* /*window*/, int width, int height)
{
    int xOffset = 0;
    int yOffset = 0;

    if (height > width)
    {
        yOffset += (height - width) / 2;
        height = width;
    }
    else if (width > height)
    {
        xOffset += (width - height) / 2;
        width = height;
    }

    glViewport(xOffset, yOffset, width, height);
}

void resetWindowViewport(GLFWwindow* window)
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    onWindowResize(window, width, height);
}