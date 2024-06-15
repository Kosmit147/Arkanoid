/// @file board.h
/// @brief Functionalities related to game board.

#pragma once

#include <glad/glad.h>
#include <stddef.h>
#include <stdbool.h>

#include "entities.h"
#include "quad_tree.h"
#include "defines.h"

/// @brief Forward declaration of Renderer struct.
typedef struct Renderer Renderer;

/// @brief Forward declaration of GameState struct.
typedef struct GameState GameState;

/// @brief Enumeration for axis types.
typedef enum Axis
{
    AXIS_VERTICAL,
    AXIS_HORIZONTAL,
} Axis;

/// @brief Structure representing the game board.
typedef struct Board
{
    Block paddle;
    QuadTree blocksQuadTree;
    size_t initialBlockCount;
    Ball ball;

    // PRIVATE
    Block* blocksStorage;
    Vector tmpRetrievedBlocksStorage;
} Board;

/// @brief Normalize a coordinate from the game coordinate space to the OpenGL coordinate space.
/// @param coord The coordinate to normalize.
/// @return The normalized coordinate.
static inline float normalizeCoordinate(float coord) { return coord / (float)COORDINATE_SPACE * 2.0f - 1.0f; }

/// @brief Normalize a length from the game coordinate space to the OpenGL coordinate space.
/// @param length The length to normalize.
/// @return The normalized length.
static inline float normalizeLength(float length) { return length / (float)COORDINATE_SPACE * 2.0f; }

/// @brief Normalize a point from the game coordinate space to the OpenGL coordinate space.
/// @param point The point to normalize.
/// @return The normalized point.
Vec2 normalizePoint(Vec2 point);

/// @brief Normalize a rectangle from the game coordinate space to the OpenGL coordinate space.
/// @param rect The rectangle to normalize.
/// @return The normalized rectangle.
Rect normalizeRect(Rect rect);

/// @brief Normalize the bounds of a rectangle from the game coordinate space to the OpenGL coordinate space.
/// @param rect The rectangle bounds to normalize.
/// @return The normalized rectangle bounds.
RectBounds normalizeRectBounds(RectBounds rect);

/// @brief Initialize the game board for a specific level.
/// @param board The board to initialize.
/// @param level The level to initialize the board for.
void initBoard(Board* board, unsigned int level);

/// @brief Check if the ball is out of bounds.
/// @param ball The ball to check.
/// @return True if the ball is out of bounds, false otherwise.
static inline bool ballOutOfBounds(const Ball* ball) { return ball->position.y + ball->radius < 0.0f; }

/// @brief Move the ball based on its velocity.
/// @param ball The ball to move.
void moveBall(Ball* ball);

/// @brief Handle the collision of the ball with the game board and other elements.
/// @param state The current game state.
/// @param board The game board.
/// @param renderer The renderer.
void collideBall(GameState* state, Board* board, Renderer* renderer);

/// @brief Free the resources used by the board.
/// @param board The board to free.
void freeBoard(Board* board);
