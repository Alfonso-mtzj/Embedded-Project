
#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include <stdint.h>
#include "../game/tictactoe.h"

typedef enum {
    MODE_PVP = 0,
    MODE_PVC = 1
} GameMode;

typedef enum {
    APP_MENU = 0,
    APP_PLAYING = 1
} AppState;

typedef struct {
    // Two simultaneous games (A and B)
    GameState games[2];
    uint8_t active_game;    // 0=A, 1=B

    // Mode
    GameMode mode;
    AppState state;

    // Cursor (shared)
    uint8_t cursor_row;     // 0..2
    uint8_t cursor_col;     // 0..2
} GameManager;

void GameManager_Init(GameManager* gm);
void GameManager_Tick(GameManager* gm);

#endif
