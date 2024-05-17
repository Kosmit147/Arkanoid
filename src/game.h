#pragma once

#include "game_state.h"
#include "board.h"
#include "rendering.h"

typedef struct Game
{
    GameState state;
    Board board;
    Renderer renderer;
} Game;

static inline bool gameOver(const Game* game) { return ballOutOfBounds(&game->board.ball); }

void initGame(Game* game, unsigned int level);
void advanceLevel(Game* game);
void freeGame(const Game* game);

void processGameInput(Game* game, GLFWwindow* window);
void moveGameObjects(Game* game);
void collideGameObjects(Game* game);
