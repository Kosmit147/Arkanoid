#include <stdbool.h>

#include "input.h"
#include "defines.h"

extern float deltaTime;

void moveBall(Ball* ball, Block* paddle, GLFWwindow* window)
{
    static bool ballLaunched = false;

    if (!ballLaunched)
    {
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            ball->translation.x = BALL_LAUNCH_TRANSLATION_X;
            ball->translation.y = BALL_LAUNCH_TRANSLATION_Y;
            ballLaunched = true;
        }

        ball->position.x = paddle->position.x + paddle->width / 2.0f;
    }

    ball->position.x += ball->translation.x * deltaTime;
    ball->position.y += ball->translation.y * deltaTime;
}

void movePaddle(Block* paddle, GLFWwindow* window)
{
    if (paddle->position.x <= 0.f)
        paddle->position.x = 0.f;

    if (paddle->position.x + paddle->width >= COORDINATE_SPACE)
        paddle->position.x = COORDINATE_SPACE - paddle->width;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        paddle->position.x += PADDLE_SPEED * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        paddle->position.x -= PADDLE_SPEED * deltaTime;

}