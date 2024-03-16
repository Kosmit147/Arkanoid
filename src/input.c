#include "input.h"
#include "rendering.h"

void movePaddle(Block *paddle, unsigned int paddleVB, GLFWwindow *window, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        paddle->position.x += PADDLE_SPEED * deltaTime;
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        paddle->position.x -= PADDLE_SPEED * deltaTime;

    updateBlockVB(paddle, paddleVB);
}