#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

#include "window.h"
#include "game.h"
#include "game_time.h"
#include "log.h"
#include "board.h"
#include "rendering.h"

#include "defines.h"

int main()
{
    srand((unsigned int)time(NULL));
    stbi_set_flip_vertically_on_load(true);

    // GLFWwindow* window = initWindowAndContext();
    GLFWwindow* window = setUpWindow("Arkanoid", WINDOW_WIDTH, WINDOW_HEIGHT);

    if (!window)
        return EXIT_FAILURE;

    if (!loadGlad())
        return EXIT_FAILURE;

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

        updateRenderer(&game.renderer, &game.board);
        render(&game.renderer, &game.state, &game.board);

        if (game.state.boardCleared)
            advanceLevel(&game);

        if (gameOver(&game))
        {
            game.state.gameOver = true;

            if (glfwGetKey(window, RESTART_GAME_KEY) == GLFW_PRESS)
            {
                // if RESTART_GAME_KEY is the same as LAUNCH_BALL_KEY then 
                // the ball would launch instantly after restarting the game
                // this is a hacky solution
                while (glfwGetKey(window, RESTART_GAME_KEY) == GLFW_PRESS)
                    glfwPollEvents();

                // TODO: write high score to file

                // TODO: refactor into restartGame function

                freeGame(&game);
                initGame(&game, STARTING_LEVEL);
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    freeGame(&game);

    glfwTerminate();
    return EXIT_SUCCESS;
}
