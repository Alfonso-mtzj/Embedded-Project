#include "msp430fr6989.h"
#include "display.h"
#include <stdio.h>

// Grid starts at x=10, y=10
// Each cell is exactly 36px wide/tall
// Dividers at x=46, x=82 (10 + 36, 10 + 72)
// Dividers at y=46, y=82
// Grid right edge = 10 + 108 = 118
// Grid bottom edge = 10 + 108 = 118

#define GRID_LEFT   10
#define GRID_TOP    10
#define GRID_RIGHT  118
#define GRID_BOT    118
#define CELL_SIZE   36
#define DIV_X1      (GRID_LEFT + CELL_SIZE)       // 46
#define DIV_X2      (GRID_LEFT + CELL_SIZE * 2)   // 82
#define DIV_Y1      (GRID_TOP  + CELL_SIZE)       // 46
#define DIV_Y2      (GRID_TOP  + CELL_SIZE * 2)   // 82
#define MARK_SIZE   22
#define CIRCLE_R    10

#define MK_RED      GRAPHICS_COLOR_RED
#define MK_GOLD     GRAPHICS_COLOR_GOLD
#define MK_CYAN     GRAPHICS_COLOR_CYAN
#define MK_ORANGE   GRAPHICS_COLOR_ORANGE
#define MK_WHITE    GRAPHICS_COLOR_WHITE
#define MK_BLACK    GRAPHICS_COLOR_BLACK

void Initialize_Clock_System(void)
{
    FRCTL0  = FRCTLPW | NWAITS_1;
    CSCTL0  = CSKEY;
    CSCTL1 &= ~DCOFSEL_7;
    CSCTL1 |=  DCOFSEL_4 | DCORSEL;
    CSCTL3 &= ~(DIVS2 | DIVS1 | DIVS0);
    CSCTL3 &= ~(DIVM2 | DIVM1 | DIVM0);
    CSCTL0_H = 0;
}

void display_init(Graphics_Context *ctx)
{
    Crystalfontz128x128_Init();
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);
    Graphics_initContext(ctx, &g_sCrystalfontz128x128);
    Graphics_setBackgroundColor(ctx, MK_BLACK);
    Graphics_setForegroundColor(ctx, MK_WHITE);
    GrContextFontSet(ctx, &g_sFontFixed6x8);
    Graphics_clearDisplay(ctx);
}

void showWelcomeScreen(Graphics_Context *ctx)
{
    Graphics_clearDisplay(ctx);
    Graphics_setForegroundColor(ctx, MK_RED);
    Graphics_drawStringCentered(ctx, "** TIC TAC TOE **",
                                AUTO_STRING_LENGTH, 64, 25, OPAQUE_TEXT);
    Graphics_setForegroundColor(ctx, MK_WHITE);
    Graphics_drawStringCentered(ctx, "Move: Joystick",
                                AUTO_STRING_LENGTH, 64, 55, OPAQUE_TEXT);
    Graphics_drawStringCentered(ctx, "Place: Push stick",
                                AUTO_STRING_LENGTH, 64, 68, OPAQUE_TEXT);
    Graphics_drawStringCentered(ctx, "Switch: S1 button",
                                AUTO_STRING_LENGTH, 64, 81, OPAQUE_TEXT);
    Graphics_drawStringCentered(ctx, "Menu:   S2 button",
                                AUTO_STRING_LENGTH, 64, 94, OPAQUE_TEXT);
    Graphics_setForegroundColor(ctx, MK_GOLD);
    Graphics_drawStringCentered(ctx, "Press S1 to start",
                                AUTO_STRING_LENGTH, 64, 114, OPAQUE_TEXT);
    Graphics_setForegroundColor(ctx, MK_WHITE);
}

void Menu(Graphics_Context *ctx, int selection)
{
    Graphics_clearDisplay(ctx);
    Graphics_setForegroundColor(ctx, MK_RED);
    Graphics_drawStringCentered(ctx, "-- Game Mode --",
                                AUTO_STRING_LENGTH, 64, 12, OPAQUE_TEXT);
    Graphics_setForegroundColor(ctx, MK_WHITE);
    Graphics_drawStringCentered(ctx, "Player vs Player",
                                AUTO_STRING_LENGTH, 64, 45, OPAQUE_TEXT);
    Graphics_drawStringCentered(ctx, "Player vs CPU",
                                AUTO_STRING_LENGTH, 64, 75, OPAQUE_TEXT);
    Graphics_drawStringCentered(ctx, "Two Games",
                                AUTO_STRING_LENGTH, 64, 105, OPAQUE_TEXT);
    Graphics_setForegroundColor(ctx, MK_GOLD);
    if(selection == 0)
        Graphics_drawString(ctx, "->", AUTO_STRING_LENGTH, 4, 42, OPAQUE_TEXT);
    else if(selection == 1)
        Graphics_drawString(ctx, "->", AUTO_STRING_LENGTH, 4, 72, OPAQUE_TEXT);
    else if(selection == 2)
        Graphics_drawString(ctx, "->", AUTO_STRING_LENGTH, 4, 102, OPAQUE_TEXT);
    Graphics_setForegroundColor(ctx, MK_WHITE);
}

void Mult_game(Graphics_Context *ctx, int selection)
{
    Graphics_clearDisplay(ctx);
    Graphics_setForegroundColor(ctx, MK_RED);
    Graphics_drawStringCentered(ctx, "-- Two Games --",
                                AUTO_STRING_LENGTH, 64, 12, OPAQUE_TEXT);
    Graphics_setForegroundColor(ctx, MK_WHITE);
    Graphics_drawStringCentered(ctx, "Both vs Player",
                                AUTO_STRING_LENGTH, 64, 45, OPAQUE_TEXT);
    Graphics_drawStringCentered(ctx, "Both vs CPU",
                                AUTO_STRING_LENGTH, 64, 75, OPAQUE_TEXT);
    Graphics_drawStringCentered(ctx, "One each mode",
                                AUTO_STRING_LENGTH, 64, 105, OPAQUE_TEXT);
    Graphics_setForegroundColor(ctx, MK_GOLD);
    if(selection == 0)
        Graphics_drawString(ctx, "->", AUTO_STRING_LENGTH, 4, 42, OPAQUE_TEXT);
    else if(selection == 1)
        Graphics_drawString(ctx, "->", AUTO_STRING_LENGTH, 4, 72, OPAQUE_TEXT);
    else if(selection == 2)
        Graphics_drawString(ctx, "->", AUTO_STRING_LENGTH, 4, 102, OPAQUE_TEXT);
    Graphics_setForegroundColor(ctx, MK_WHITE);
}

void drawBoard(Graphics_Context *ctx)
{
    Graphics_clearDisplay(ctx);
    Graphics_setForegroundColor(ctx, MK_RED);
    Graphics_drawLine(ctx, DIV_X1, GRID_TOP, DIV_X1, GRID_BOT);
    Graphics_drawLine(ctx, DIV_X2, GRID_TOP, DIV_X2, GRID_BOT);
    Graphics_drawLine(ctx, GRID_LEFT, DIV_Y1, GRID_RIGHT, DIV_Y1);
    Graphics_drawLine(ctx, GRID_LEFT, DIV_Y2, GRID_RIGHT, DIV_Y2);
    Graphics_setForegroundColor(ctx, MK_WHITE);
}

void clearBoard(char grid[3][3])
{
    int r, c;
    for(r = 0; r < 3; r++)
        for(c = 0; c < 3; c++)
            grid[r][c] = ' ';
}

void drawMarks(Graphics_Context *ctx, char grid[3][3])
{
    int r, c;
    for(r = 0; r < 3; r++)
        for(c = 0; c < 3; c++)
        {
            if(grid[r][c] == 'X') drawX(ctx, r, c);
            if(grid[r][c] == 'O') drawO(ctx, r, c);
        }
}

void drawX(Graphics_Context *ctx, int r, int c)
{
    int x0 = GRID_LEFT + (c * CELL_SIZE) + (CELL_SIZE - MARK_SIZE) / 2;
    int y0 = GRID_TOP  + (r * CELL_SIZE) + (CELL_SIZE - MARK_SIZE) / 2;
    Graphics_setForegroundColor(ctx, MK_CYAN);
    Graphics_drawLine(ctx, x0,             y0,
                           x0 + MARK_SIZE, y0 + MARK_SIZE);
    Graphics_drawLine(ctx, x0 + MARK_SIZE, y0,
                           x0,             y0 + MARK_SIZE);
    Graphics_setForegroundColor(ctx, MK_WHITE);
}

void drawO(Graphics_Context *ctx, int r, int c)
{
    int cx = GRID_LEFT + (c * CELL_SIZE) + CELL_SIZE / 2;
    int cy = GRID_TOP  + (r * CELL_SIZE) + CELL_SIZE / 2;
    Graphics_setForegroundColor(ctx, MK_ORANGE);
    Graphics_drawCircle(ctx, cx, cy, CIRCLE_R);
    Graphics_setForegroundColor(ctx, MK_WHITE);
}

void drawCursor(Graphics_Context *ctx, int r, int c)
{
    int x = GRID_LEFT + (c * CELL_SIZE) + 2;
    int y = GRID_TOP  + (r * CELL_SIZE) + 2;
    Graphics_Rectangle box;
    box.xMin = x;
    box.xMax = x + CELL_SIZE - 4;
    box.yMin = y;
    box.yMax = y + CELL_SIZE - 4;
    Graphics_setForegroundColor(ctx, MK_GOLD);
    Graphics_drawRectangle(ctx, &box);
    Graphics_setForegroundColor(ctx, MK_WHITE);
}

void clearCell(Graphics_Context *ctx, int r, int c)
{
    int x = GRID_LEFT + (c * CELL_SIZE) + 1;
    int y = GRID_TOP  + (r * CELL_SIZE) + 1;
    Graphics_Rectangle box;
    box.xMin = x;
    box.xMax = x + CELL_SIZE - 1;
    box.yMin = y;
    box.yMax = y + CELL_SIZE - 1;
    Graphics_setForegroundColor(ctx, MK_BLACK);
    Graphics_fillRectangle(ctx, &box);
    Graphics_setForegroundColor(ctx, MK_WHITE);
}

void showTurn(Graphics_Context *ctx, char player)
{
    Graphics_setForegroundColor(ctx, MK_RED);
    if(player == 'X')
        Graphics_drawStringCentered(ctx, "Turn: X",
                                    AUTO_STRING_LENGTH, 64, 124, OPAQUE_TEXT);
    else
        Graphics_drawStringCentered(ctx, "Turn: O",
                                    AUTO_STRING_LENGTH, 64, 124, OPAQUE_TEXT);
    Graphics_setForegroundColor(ctx, MK_WHITE);
}

void showGameNumber(Graphics_Context *ctx, int num)
{
    Graphics_setForegroundColor(ctx, MK_GOLD);
    if(num == 1)
        Graphics_drawStringCentered(ctx, "Game A",
                                    AUTO_STRING_LENGTH, 64, 5, OPAQUE_TEXT);
    else
        Graphics_drawStringCentered(ctx, "Game B",
                                    AUTO_STRING_LENGTH, 64, 5, OPAQUE_TEXT);
    Graphics_setForegroundColor(ctx, MK_WHITE);
}

void showWinner(Graphics_Context *ctx, char winner)
{
    Graphics_clearDisplay(ctx);
    Graphics_setForegroundColor(ctx, MK_GOLD);
    if(winner == 'X')
        Graphics_drawStringCentered(ctx, "X wins!",
                                    AUTO_STRING_LENGTH, 64, 50, OPAQUE_TEXT);
    else
        Graphics_drawStringCentered(ctx, "O wins!",
                                    AUTO_STRING_LENGTH, 64, 50, OPAQUE_TEXT);
    Graphics_setForegroundColor(ctx, MK_WHITE);
    Graphics_drawStringCentered(ctx, "Resetting...",
                                AUTO_STRING_LENGTH, 64, 75, OPAQUE_TEXT);
}

void showTie(Graphics_Context *ctx)
{
    Graphics_clearDisplay(ctx);
    Graphics_setForegroundColor(ctx, MK_ORANGE);
    Graphics_drawStringCentered(ctx, "It's a draw!",
                                AUTO_STRING_LENGTH, 64, 50, OPAQUE_TEXT);
    Graphics_setForegroundColor(ctx, MK_WHITE);
    Graphics_drawStringCentered(ctx, "Resetting...",
                                AUTO_STRING_LENGTH, 64, 75, OPAQUE_TEXT);
}

void showLoser(Graphics_Context *ctx)
{
    Graphics_clearDisplay(ctx);
    Graphics_setForegroundColor(ctx, MK_RED);
    Graphics_drawStringCentered(ctx, "CPU wins!",
                                AUTO_STRING_LENGTH, 64, 45, OPAQUE_TEXT);
    Graphics_setForegroundColor(ctx, MK_WHITE);
    Graphics_drawStringCentered(ctx, "Better luck",
                                AUTO_STRING_LENGTH, 64, 68, OPAQUE_TEXT);
    Graphics_drawStringCentered(ctx, "next time!",
                                AUTO_STRING_LENGTH, 64, 81, OPAQUE_TEXT);
}

void redrawCellLines(Graphics_Context *ctx, int r, int c)
{
    int x = GRID_LEFT + (c * CELL_SIZE);
    int y = GRID_TOP  + (r * CELL_SIZE);
    Graphics_setForegroundColor(ctx, MK_RED);
    if(c > 0) Graphics_drawLine(ctx, x,             GRID_TOP, x,             GRID_BOT);
    if(c < 2) Graphics_drawLine(ctx, x + CELL_SIZE, GRID_TOP, x + CELL_SIZE, GRID_BOT);
    if(r > 0) Graphics_drawLine(ctx, GRID_LEFT, y,             GRID_RIGHT, y);
    if(r < 2) Graphics_drawLine(ctx, GRID_LEFT, y + CELL_SIZE, GRID_RIGHT, y + CELL_SIZE);
    Graphics_setForegroundColor(ctx, MK_WHITE);
}
