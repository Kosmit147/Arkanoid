#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "game_state.h"
#include "entities.h"

void processBallLaunchInput(GameState* state, Ball* ball, const Block* paddle, GLFWwindow* window);
void processPaddleMovementInput(Block* paddle, GLFWwindow* window);
