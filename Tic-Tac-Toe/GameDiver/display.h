#ifndef DISPLAY_H
#define DISPLAY_H

#include "Grlib/grlib/grlib.h"
#include "LcdDriver/lcd_driver.h"

// System setup
void Initialize_Clock_System(void);
void display_init(Graphics_Context *ctx);

// Screens
void showWelcomeScreen(Graphics_Context *ctx);
void showWinner(Graphics_Context *ctx, char winner);
void showTie(Graphics_Context *ctx);
void showLoser(Graphics_Context *ctx);

// Menu screens
void Menu(Graphics_Context *ctx, int selection);
void Mult_game(Graphics_Context *ctx, int selection);

// Board drawing
void drawBoard(Graphics_Context *ctx);
void clearBoard(char grid[3][3]);
void drawMarks(Graphics_Context *ctx, char grid[3][3]);
void drawX(Graphics_Context *ctx, int r, int c);
void drawO(Graphics_Context *ctx, int r, int c);
void drawCursor(Graphics_Context *ctx, int r, int c);
void clearCell(Graphics_Context *ctx, int r, int c);

// Game info display
void showTurn(Graphics_Context *ctx, char player);
void showGameNumber(Graphics_Context *ctx, int num);

void redrawCellLines(Graphics_Context *ctx, int r, int c);

#endif

