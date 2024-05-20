#include "game.h"

#include <stdio.h>

#include "input.h"
#include "rendering.h"
#include "str_utils.h"

void initGame(Game* game, unsigned int level)
{
    initGameState(&game->state, level);
    initBoard(&game->board, game->state.level);
    initRenderer(&game->renderer, &game->board);
}

// returns the length of the created string
static size_t createLevelStr(char* str, size_t buffSize, unsigned int points)
{
    strcpy(str, LEVEL_STR);
    size_t charsWritten = (size_t)snprintf(str + staticStrLen(LEVEL_STR),
        buffSize - staticStrLen(LEVEL_STR), "%u", points);

    if (charsWritten > buffSize - staticStrLen(LEVEL_STR) - 1)
        charsWritten = buffSize - staticStrLen(LEVEL_STR) - 1;

    return staticStrLen(LEVEL_STR) + charsWritten;
}

void advanceLevel(Game* game)
{
    freeBoard(&game->board);
    freeGameRenderer(&game->renderer.gameRenderer);

    gameStateAdvanceLevel(&game->state);
    initBoard(&game->board, game->state.level);
    initGameRenderer(&game->renderer.gameRenderer, &game->board, game->renderer.quadIB);

    char levelText[staticStrLen(LEVEL_STR) + MAX_DIGITS_IN_LEVEL_NUM + 1];
    size_t levelTextLen = createLevelStr(levelText,
        staticStrLen(LEVEL_STR) + MAX_DIGITS_IN_LEVEL_NUM + 1, game->state.level);

    TextRenderer* levelRenderer = &game->renderer.hudRenderer.levelRenderer;
    updateTextRenderer(levelRenderer, levelText, levelTextLen, levelRenderer->position);
}

void freeGame(const Game* game)
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
