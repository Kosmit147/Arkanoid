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
    glDebugMessageCallback(GLDebugCallback, NULL);
    logNotification("%s\n", (const char*)glGetString(GL_VERSION));
#endif

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GameState state = {
        .ballLaunched = false,
    };

    GameObjects objects = createGameObjects();
    RenderingData renderingData = createRenderingData(&objects);

    removeBlock(objects.blocks, &objects.blockCount, 1);
    removeBlock(objects.blocks, &objects.blockCount, 2);
    removeBlock(objects.blocks, &objects.blockCount, 3);
    updateBlocksVBOnBlocksDestroyed(renderingData.blocksBuffers.VB, 1, 3, objects.blockCount);

    float prevTime = (float)glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        time = (float)glfwGetTime();
        deltaTime = min(time - prevTime, DELTA_TIME_LIMIT);

        glClear(GL_COLOR_BUFFER_BIT);

        processInput(&state, &objects, window);
        moveGameObjects(&objects);
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