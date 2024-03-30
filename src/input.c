#include "input.h"
#include <stdbool.h>
#include "defines.h"

void ballLaunched(Ball* ball, Block* paddle, GLFWwindow* window)
{
    static bool shootFlag = false;
    if (!shootFlag) {
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            ball->translation.x = BALL_LAUNCH_TRANSLATION_X;
            ball->translation.y = BALL_LAUNCH_TRANSLATION_Y;
            shootFlag = true;
        }
        ball->position.x = paddle->position.x + paddle->width / 2;
    }
}

void moveBall(Ball* ball, float deltaTime)
{
    ball->position.x += ball->translation.x * deltaTime;
    ball->position.y += ball->translation.y * deltaTime;
}

void movePaddle(Block* paddle, GLFWwindow* window, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        paddle->position.x += PADDLE_SPEED * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        paddle->position.x -= PADDLE_SPEED * deltaTime;
}