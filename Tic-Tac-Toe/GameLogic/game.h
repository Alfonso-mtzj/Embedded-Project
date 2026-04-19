// game.h
// Defines the board symbols, game modes, and the GameState struct.
// Also declares all the game functions used by main.c.

#ifndef GAME_H
#define GAME_H

#include "Grlib/grlib/grlib.h"

// board cell values
#define EMPTY    ' '
#define PLAYER_X 'X'
#define PLAYER_O 'O'

// single game mode codes returned by the menu
#define MODE_PVP    0
#define MODE_PVC    1
#define MODE_DUAL   2

// struct used to pass board state to the AI
typedef struct {
    char board[3][3];     // the 3x3 grid
    char currentPlayer;   // whose turn it is
} GameState;

// game setup and mode functions
void game_setup(void);
void run_pvp(Graphics_Context *ctx);
void run_pvc(Graphics_Context *ctx);
void run_dual_pvp(Graphics_Context *ctx);
void run_dual_pvc(Graphics_Context *ctx);
void run_dual_mixed(Graphics_Context *ctx);

// shared flags - changed by interrupts in main.c
extern volatile int activeBoard;  // which board is showing (1 or 2)
extern volatile int needsRedraw;  // 1 = redraw the screen
extern volatile int exitToMenu;   // 1 = go back to the menu

#endif
