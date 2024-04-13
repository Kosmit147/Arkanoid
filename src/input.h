#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "game_state.h"
#include "board.h"

void processInput(GameState* state, GameObjects* gameObjects, GLFWwindow* window);