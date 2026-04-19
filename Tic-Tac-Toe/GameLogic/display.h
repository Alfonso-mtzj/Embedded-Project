// display.h
// Declares all the functions used to draw on the LCD screen.

#ifndef DISPLAY_H
#define DISPLAY_H

#include "Grlib/grlib/grlib.h"
#include "LcdDriver/lcd_driver.h"

// clock and display setup
void Initialize_Clock_System(void);
void display_init(Graphics_Context *ctx);

// welcome and end screens
void showWelcomeScreen(Graphics_Context *ctx);
void showWinner(Graphics_Context *ctx, char winner);
void showTie(Graphics_Context *ctx);
void showLoser(Graphics_Context *ctx);

// menu screens
void Menu(Graphics_Context *ctx, int selection);
void Mult_game(Graphics_Context *ctx, int selection);

// board drawing
void drawBoard(Graphics_Context *ctx);
void clearBoard(char grid[3][3]);
void drawMarks(Graphics_Context *ctx, char grid[3][3]);
void drawX(Graphics_Context *ctx, int r, int c);
void drawO(Graphics_Context *ctx, int r, int c);
void drawCursor(Graphics_Context *ctx, int r, int c);
void clearCell(Graphics_Context *ctx, int r, int c);
void redrawCellLines(Graphics_Context *ctx, int r, int c);

// in-game info
void showTurn(Graphics_Context *ctx, char player);
void showGameNumber(Graphics_Context *ctx, int num);

#endif
