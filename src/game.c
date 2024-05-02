#include "game.h"

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
