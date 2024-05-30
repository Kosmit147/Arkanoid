#include "game.h"

#include "input.h"
#include "rendering.h"

void initGame(Game* game, unsigned int level)
{
    initGameState(&game->state, level);
    initBoard(&game->board, game->state.level);
    initRenderer(&game->renderer, &game->board);
}

void advanceLevel(Game* game)
{
    freeBoard(&game->board);
    freeGameRenderer(&game->renderer.gameRenderer);

    gameStateAdvanceLevel(&game->state);
    initBoard(&game->board, game->state.level);
    initGameRenderer(&game->renderer.gameRenderer, &game->board, game->renderer.quadIB);
    updateHudLevelText(&game->renderer.hudRenderer, game->state.level);

// TODO: Factor out into hudAdvanceLevel function
#ifdef DRAW_QUAD_TREE
    redrawQuadTree(&game->renderer.hudRenderer, &game->board.blocksQuadTree);
#endif
}

void freeGame(Game* game)
{
    freeBoard(&game->board);
    freeRenderer(&game->renderer);
}

void processGameInput(Game* game, GLFWwindow* window)
{
    processPaddleMovementInput(&game->board.paddle, window);
    processBallLaunchInput(&game->state, &game->board.ball, &game->board.paddle, window);
}

void moveGameObjects(Game* game)
{
    moveBall(&game->board.ball);
}

void collideGameObjects(Game* game)
{
    collideBall(&game->state, &game->board, &game->renderer);
}
