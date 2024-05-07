#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <time.h>

#include "window.h"
#include "game.h"
#include "game_time.h"
#include "helpers.h"
#include "log.h"
#include "board.h"
#include "input.h"
#include "rendering.h"

#include "defines.h"

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

    Game game;
    initGame(&game, STARTING_LEVEL);

    initTime();

    while (!glfwWindowShouldClose(window))
    {
        updateTime();
        glClear(GL_COLOR_BUFFER_BIT);

        for (size_t i = 0; i < SIMULATION_SUB_STEPS; i++)
        {
            processGameInput(&game, window);
            moveGameObjects(&game);
            collideGameObjects(&game);
        }

        updateRenderData(&game.renderData, &game.objects);
        render(&game.renderData, &game.objects);

        if (gameBoardCleared(&game.objects))
            advanceLevel(&game);

        if (gameOver(&game) && glfwGetKey(window, GAME_OVER_START_NEW_GAME_KEY) == GLFW_PRESS)
        {
            // if GAME_OVER_START_NEW_GAME_KEY is the same as LAUNCH_BALL_KEY then 
            // the ball would launch instantly after restarting the game
            // this is a hacky solution
            while (glfwGetKey(window, GAME_OVER_START_NEW_GAME_KEY) == GLFW_PRESS)
                glfwPollEvents();

            // TODO: write high score to file

            freeGame(&game);
            initGame(&game, STARTING_LEVEL);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    freeGame(&game);

    glfwTerminate();
    return 0;
}
