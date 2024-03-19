#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "entities.h"

void movePaddle(Block* paddle, GLFWwindow* window, float deltaTime);

void moveBall(Ball* ball, float deltaTime);

void launchBall(Ball* ball, Block* paddle, GLFWwindow* window);

void moveBallBeforeLaunch(Block* paddle, Ball* ball);