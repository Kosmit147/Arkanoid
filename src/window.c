#include "window.h"

#include <stdbool.h>

#include "helpers.h"
#include "log.h"

#include "defines.h"

#ifdef _DEBUG
void rendererGLDebugCallback(GLenum UNUSED(source), GLenum UNUSED(type), GLuint UNUSED(id), GLenum severity,
    GLsizei UNUSED(length), const GLchar* message, const void* UNUSED(userParam))
{
    if (ARKANOID_GL_DEBUG_MESSAGE_MIN_SEVERITY != GL_DEBUG_SEVERITY_NOTIFICATION &&
        severity > ARKANOID_GL_DEBUG_MESSAGE_MIN_SEVERITY)
    {
        return;
    }

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        logNotification("[OpenGL Notification]: %s.\n", message);
        break;
    case GL_DEBUG_SEVERITY_LOW:
    case GL_DEBUG_SEVERITY_MEDIUM:
        logWarning("[OpenGL Warning]: %s.\n", message);
        break;
    case GL_DEBUG_SEVERITY_HIGH:
        logError("[OpenGL Error]: %s.\n", message);
        break;
    }
}
#endif

static GLFWwindow* setUpWindow(const char* title, int width, int height)
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef _DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);

    if (!window)
        return NULL;

    glfwSetWindowAspectRatio(window, 1, 1);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    return window;
}

// glfw must be initialized beforehand
static bool loadGlad()
{
    return gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
}

static void onWindowResize(GLFWwindow* UNUSED(window), int width, int height)
{
    GLint xOffset = 0;
    GLint yOffset = 0;

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

static void initGLViewport(GLFWwindow* window)
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);

#ifdef _GLFW_WAYLAND
    float xScale, yScale;
    glfwGetWindowContentScale(window, &xScale, &yScale);

    width = (int)((float)width * xScale);
    height = (int)((float)height * yScale);
#endif

    onWindowResize(window, width, height);
}

GLFWwindow* setUpWindowAndContext(const char* title, int width, int height)
{
    if (!glfwInit())
    {
        logError("Failed to initialize GLFW.\n");
        exit(EXIT_FAILED_TO_INIT_GLFW);
    }

    GLFWwindow* window = setUpWindow(title, width, height);

    if (!window)
    {
        logError("Failed to create a window.\n");
        glfwTerminate();
        exit(EXIT_FAILED_TO_CREATE_A_WINDOW);
    }

    if (!loadGlad())
    {
        logError("Failed to initialize GLAD.\n");
        glfwTerminate();
        exit(EXIT_FAILED_TO_INIT_GLAD);
    }

    initGLViewport(window);
    glfwSetFramebufferSizeCallback(window, onWindowResize);

#ifdef _DEBUG
    glDebugMessageCallback(rendererGLDebugCallback, NULL);
    logNotification("%s\n", glGetString(GL_VERSION));
#endif

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return window;
}
