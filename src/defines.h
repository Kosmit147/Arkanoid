#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <float.h>

#include "defines_shared.h"

#define MATH_PI 3.14159265358979323846
#define RADIANS_IN_DEG 0.01745329252

#define ARKANOID_GL_DEBUG_MESSAGE_MIN_SEVERITY GL_DEBUG_SEVERITY_NOTIFICATION
#define ARKANOID_GL_SHADER_VERSION_DECL "#version 430 core\n"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

#define DELTA_TIME_LIMIT 0.035f
#define SIMULATION_SUB_STEPS 16

#define COORDINATE_SCALING (COORDINATE_SPACE / 1000.0f)

#define PADDLE_WIDTH (180.0f * COORDINATE_SCALING)
#define PADDLE_HEIGHT (40.0f * COORDINATE_SCALING)
#define PADDLE_START_POS_X (COORDINATE_SPACE / 2.0f - PADDLE_WIDTH / 2.0f)
#define PADDLE_START_POS_Y (80.0f * COORDINATE_SCALING)
#define PADDLE_SPEED (500.0f * COORDINATE_SCALING)
#define PADDLE_COLOR ((Vec4){ .r = 0.2f, .g = 0.2f, .b = 1.0f, .a = 1.0f })

#define BALL_RADIUS (25.0f * COORDINATE_SCALING)
#define BALL_START_POS_X (PADDLE_START_POS_X + PADDLE_WIDTH / 2.0f)
#define BALL_START_POS_Y (PADDLE_START_POS_Y + BALL_RADIUS + FLT_EPSILON * 2.0f)
#define BALL_LAUNCH_DIRECTION_X 0.0f
#define BALL_LAUNCH_DIRECTION_Y 1.0f
// ball speed should be higher than paddle speed for collisions to work properly
#define BALL_LAUNCH_SPEED (600.0f * COORDINATE_SCALING)
#define BALL_COLOR ((Vec4){ .r = 1.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f })
#define MIN_BALL_BOUNCE_ANGLE_OFF_PADDLE (5 * RADIANS_IN_DEG)

#ifdef _DEBUG
#define STARTING_LEVEL 0
#else
#define STARTING_LEVEL 1
#endif

#define POINTS_PER_BLOCK_DESTROYED 10

#define BLOCK_CHAR '#'
#define BLOCK_HORIZONTAL_PADDING (10.0f * COORDINATE_SCALING)
#define BLOCK_VERTICAL_PADDING (10.0f * COORDINATE_SCALING)
#define BLOCK_BORDER_WIDTH (5.0f * COORDINATE_SCALING)

#define MOVE_PADDLE_LEFT_KEY            GLFW_KEY_A
#define MOVE_PADDLE_LEFT_KEY_ALT        GLFW_KEY_LEFT
#define MOVE_PADDLE_RIGHT_KEY           GLFW_KEY_D
#define MOVE_PADDLE_RIGHT_KEY_ALT       GLFW_KEY_RIGHT
#define LAUNCH_BALL_KEY                 GLFW_KEY_SPACE
#define RESTART_GAME_KEY                GLFW_KEY_SPACE

#define GAME_OVER_STR "GAME OVER!"
#define PRESS_RESTART_GAME_KEY_STR "Press SPACE to restart the game."
#define POINTS_STR "Points: "

#define FONT_PROPORTIONS (3.0f / 2.0f)
#define FONT_WIDTH 0.06f
#define FONT_HEIGHT (FONT_WIDTH * FONT_PROPORTIONS)

#define MAX_QUADS 1000
#define QUAD_IB_DATA_TYPE GL_UNSIGNED_SHORT

#define TEXT_RENDERER_FLOATS_PER_CHAR_VERTEX 4
#define TEXT_RENDERER_CHAR_VERTICES_SIZE (sizeof(float) * TEXT_RENDERER_FLOATS_PER_CHAR_VERTEX * 4)
