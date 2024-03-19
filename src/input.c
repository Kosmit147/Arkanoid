#include "input.h"
#include <stdbool.h>
#include "defines.h"

void launchBall(Ball* ball, Block* paddle, GLFWwindow* window)
{
    static bool shootFlag = 0;
    if (!shootFlag) {
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            ball->translation.x = BALL_START_TRANSLATION_X;
            ball->translation.y = BALL_START_TRANSLATION_Y;
            shootFlag = 1;
        }
        moveBallBeforeLaunch(paddle, ball);
    }
}

void moveBall(Ball* ball, float deltaTime)
{
    ball->position.x += ball->translation.x * deltaTime;
    ball->position.y += ball->translation.y * deltaTime;
}

void moveBallBeforeLaunch(Block* paddle, Ball* ball) {
    ball->position.x = paddle->position.x + paddle->width / 2;
}

void movePaddle(Block* paddle, GLFWwindow* window, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        paddle->position.x += PADDLE_SPEED * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        paddle->position.x -= PADDLE_SPEED * deltaTime;
}