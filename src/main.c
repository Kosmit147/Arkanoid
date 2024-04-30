#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <time.h>

#include "helpers.h"
#include "log.h"
#include "window.h"
#include "game_state.h"
#include "board.h"
#include "input.h"
#include "rendering.h"

#include "defines.h"

float currTime;
float deltaTime;
float subStepDeltaTime;

int main()
{
    srand((unsigned int)time(NULL));

    GLFWwindow* window = setUpWindow("Arkanoid", WINDOW_WIDTH, WINDOW_HEIGHT);

    if (!window)
        return -1;

    if (!loadGlad())
        return -1;

    initGLViewport(window);
    glfwSetFramebufferSizeCallback(window, onWindowResize);

#ifdef _DEBUG
    glDebugMessageCallback(rendererGLDebugCallback, NULL);
    logNotification("%s\n", glGetString(GL_VERSION));
#endif

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GameState state = {
        .ballLaunched = false,
        .isGameOver = false,
        .points = 0.
    };

    GameObjects objects = createGameObjects();
    GameRenderData renderData;
    initRenderData(&renderData, &objects);

    float prevTime = (float)glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        if (state.isGameOver && glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        {
            resetState(&state);
            resetBoard(&objects);
            initRenderData(&renderData, &objects);
        }
        currTime = (float)glfwGetTime();
        deltaTime = min(currTime - prevTime, DELTA_TIME_LIMIT);
        subStepDeltaTime = deltaTime / SIMULATION_SUB_STEPS;

        glClear(GL_COLOR_BUFFER_BIT);

        for (size_t i = 0; i < SIMULATION_SUB_STEPS; i++)
        {
            if (state.isGameOver)
            {
                break;
            }
            processInput(&state, &objects, window);
            moveGameObjects(&objects);
            collideGameObjects(&objects, &renderData, &state);
            isGameOver(&objects.ball, &state);
        }

        updateRenderData(&renderData, &objects);
        render(&renderData, &objects);

        glfwSwapBuffers(window);
        glfwPollEvents();

        prevTime = currTime;
    }

    freeRenderData(&renderData);
    freeGameObjects(&objects);

    glfwTerminate();
    return 0;
}
