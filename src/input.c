#include <stdbool.h>

#include "input.h"
#include "defines.h"

extern float deltaTime;

static void moveBall(GameState* state, Ball* ball, const Block* paddle, GLFWwindow* window)
{
    if (!state->ballLaunched)
    {
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            ball->speed = BALL_LAUNCH_SPEED;
            state->ballLaunched = true;
        }

        ball->position.x = paddle->position.x + paddle->width / 2.0f;
    }
}

static void movePaddle(Block* paddle, GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        paddle->position.x += PADDLE_SPEED * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        paddle->position.x -= PADDLE_SPEED * deltaTime;

    if (paddle->position.x < 0.f)
        paddle->position.x = 0.f;
    else if (paddle->position.x + paddle->width > COORDINATE_SPACE)
        paddle->position.x = COORDINATE_SPACE - paddle->width;
}

void processInput(GameState* state, GameObjects* gameObjects, GLFWwindow* window)
{
    movePaddle(&gameObjects->paddle, window);
    moveBall(state, &gameObjects->ball, &gameObjects->paddle, window);
}