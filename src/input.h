/// @file input.h
/// @brief Functionality related to game response after player player clicked keyboard

#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "game_state.h"
#include "entities.h"

/// @brief Processes the input for launching the ball.
/// @param state The current game state.
/// @param ball The ball to be launched.
/// @param paddle The paddle used to launch the ball.
/// @param window The GLFW window instance.
void processBallLaunchInput(GameState* state, Ball* ball, const Block* paddle, GLFWwindow* window);

/// @brief Processes the input for moving the paddle.
/// @param paddle The paddle to be moved.
/// @param window The GLFW window instance.
void processPaddleMovementInput(Block* paddle, GLFWwindow* window);
