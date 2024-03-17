#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "entities.h"

void movePaddle(Block *paddle, unsigned int paddleVB, GLFWwindow *window, float deltaTime);