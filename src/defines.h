#pragma once

#include <glad/glad.h>

#include "defines_shared.h"

#define GL_DEBUG_MESSAGE_MIN_SEVERITY GL_DEBUG_SEVERITY_NOTIFICATION
#define GL_SHADER_VERSION_DECL "#version 430 core\n"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 1200

#define DELTA_TIME_LIMIT 0.035f

#define COORDINATE_SCALING (COORDINATE_SPACE / 1000.0f)

#define PADDLE_WIDTH (150.0f * COORDINATE_SCALING)
#define PADDLE_HEIGHT (50.0f * COORDINATE_SCALING)
#define PADDLE_START_POS_X (COORDINATE_SPACE / 2.0f - PADDLE_WIDTH / 2.0f)
#define PADDLE_START_POS_Y (100.0f * COORDINATE_SCALING)
#define PADDLE_SPEED (500.0f * COORDINATE_SCALING)

#define BALL_RADIUS (25.0f * COORDINATE_SCALING)
#define BALL_START_POS_X (PADDLE_START_POS_X + PADDLE_WIDTH / 2.0f)
#define BALL_START_POS_Y (PADDLE_START_POS_Y + BALL_RADIUS)
#define BALL_START_TRANSLATION_X (0.0f * COORDINATE_SCALING)
#define BALL_START_TRANSLATION_Y (200.0f * COORDINATE_SCALING)

#define BLOCK_CHAR '#'
#define BLOCK_HORIZONTAL_PADDING (10.0f * COORDINATE_SCALING)
#define BLOCK_VERTICAL_PADDING (10.0f * COORDINATE_SCALING)

#define FLOATS_PER_BLOCK_VERTEX 2