#pragma once

#include <glad/glad.h>

#include <stddef.h>
#include <stdbool.h>

#include "entities.h"

#include "defines.h"

typedef struct GameState GameState;
typedef struct GameRenderer GameRenderer;

typedef struct Board
{
    Block paddle;
    size_t blockCount;
    Block* blocks;
    Ball ball;
} Board;

typedef enum Axis
{
    AXIS_VERTICAL,
    AXIS_HORIZONTAL,
} Axis;

static inline float normalizeCoordinate(float coord) { return coord / (float)COORDINATE_SPACE * 2.0f - 1.0f; }
static inline float normalizeLength(float length) { return length / (float)COORDINATE_SPACE * 2.0f; }

Rect normalizeRect(Rect rect);
RectBounds normalizeRectBounds(RectBounds rect);

void initBoard(Board* board, unsigned int level);

static inline bool ballOutOfBounds(const Ball* ball) { return ball->position.y + ball->radius < 0.0f; }
static inline bool boardCleared(const Board* board) { return board->blockCount == 0; }

void moveBall(Ball* ball);
void collideBall(GameState* state, Board* board, GameRenderer* renderer);

void freeBoard(const Board* board);
