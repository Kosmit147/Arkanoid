#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "helpers.h"
#include "log.h"
#include "window.h"
#include "game_state.h"
#include "board.h"
#include "input.h"
#include "rendering.h"

#include "defines.h"

float time;
float deltaTime;
float subStepDeltaTime;

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
    glDebugMessageCallback(rendererGLDebugCallback, NULL);
    logNotification("%s\n", glGetString(GL_VERSION));
#endif

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GameState state = {
        .ballLaunched = false,
    };

    GameObjects objects = createGameObjects();
    RenderingData renderingData;
    initRenderingData(&renderingData, &objects);

    float prevTime = (float)glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        time = (float)glfwGetTime();
        deltaTime = min(time - prevTime, DELTA_TIME_LIMIT);
        subStepDeltaTime = deltaTime / SIMULATION_SUB_STEPS;

        glClear(GL_COLOR_BUFFER_BIT);

        for (size_t i = 0; i < SIMULATION_SUB_STEPS; i++)
        {
            processInput(&state, &objects, window);
            moveGameObjects(&objects);
            collideGameObjects(&objects, &renderingData);
        }

        updateRenderingData(&renderingData, &objects);
        render(&renderingData, &objects);

        glfwSwapBuffers(window);
        glfwPollEvents();

        prevTime = time;
    }

    freeRenderingData(&renderingData);
    freeGameObjects(&objects);

    glfwTerminate();
    return 0;
}
