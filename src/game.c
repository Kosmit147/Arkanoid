#include "game.h"

#include "input.h"

void initGame(Game* game, unsigned int level)
{
    initGameState(&game->state, level);

    game->objects = createGameObjects(game->state.level);
    initRenderData(&game->renderData, &game->objects);
}

void advanceLevel(Game* game)
{
    freeGameObjects(&game->objects);
    freeRenderData(&game->renderData);

    gameStateAdvanceLevel(&game->state);

    game->objects = createGameObjects(game->state.level);
    initRenderData(&game->renderData, &game->objects);
}

void freeGame(const Game* game)
{
    freeGameObjects(&game->objects);
    freeRenderData(&game->renderData);
}

void moveGameObjects(Game* game)
{
    moveBall(&game->objects.ball);
}

void collideGameObjects(Game* game)
{
    collideBall(&game->state, &game->objects, &game->renderData);
}

void processGameInput(Game* game, GLFWwindow* window)
{
    processPaddleMovementInput(&game->objects.paddle, window);
    processBallLaunchInput(&game->state, &game->objects.ball, &game->objects.paddle, window);
}
