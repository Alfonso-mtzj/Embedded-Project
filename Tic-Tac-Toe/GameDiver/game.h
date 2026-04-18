#ifndef GAME_H
#define GAME_H

#include "Grlib/grlib/grlib.h"

#define EMPTY    ' '
#define PLAYER_X 'X'
#define PLAYER_O 'O'

#define MODE_PVP    0
#define MODE_PVC    1
#define MODE_DUAL   2

typedef struct {
    char board[3][3];
    char currentPlayer;
} GameState;

void game_setup(void);
void run_pvp(Graphics_Context *ctx);
void run_pvc(Graphics_Context *ctx);
void run_dual_pvp(Graphics_Context *ctx);
void run_dual_pvc(Graphics_Context *ctx);
void run_dual_mixed(Graphics_Context *ctx);

extern volatile int activeBoard;
extern volatile int needsRedraw;
extern volatile int exitToMenu;

#endif
