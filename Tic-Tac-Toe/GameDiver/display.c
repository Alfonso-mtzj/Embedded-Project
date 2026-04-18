
#include "msp430fr6989.h"
#include "display.h"
#include <stdio.h>

// Cell size and grid offsets
#define CELL_SIZE   42
#define GRID_LEFT   10
#define GRID_TOP    10
#define GRID_RIGHT  118
#define GRID_BOTTOM 118
#define MARK_OFFSET 14
#define MARK_SIZE   20
#define CIRCLE_CX   24
#define CIRCLE_R    10

//=== Clock Setup ===

void Initialize_Clock_System(void)
{
    // Target: DCO at 16 MHz, MCLK and SMCLK both at 16 MHz
    FRCTL0  = FRCTLPW | NWAITS_1;
    CSCTL0  = CSKEY;
    CSCTL1 &= ~DCOFSEL_7;
    CSCTL1 |=  DCOFSEL_4 | DCORSEL;
    CSCTL3 &= ~(DIVS2 | DIVS1 | DIVS0);
    CSCTL3 &= ~(DIVM2 | DIVM1 | DIVM0);
    CSCTL0_H = 0;
}

//=== Display Init ===

void display_init(Graphics_Context *ctx)
{
    Crystalfontz128x128_Init();
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);
    Graphics_initContext(ctx, &g_sCrystalfontz128x128);
    Graphics_setBackgroundColor(ctx, GRAPHICS_COLOR_BLACK);
    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_WHITE);
    GrContextFontSet(ctx, &g_sFontFixed6x8);
    Graphics_clearDisplay(ctx);
}

//=== Welcome Screen ===

void showWelcomeScreen(Graphics_Context *ctx)
{
    Graphics_clearDisplay(ctx);

    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_CYAN);
    Graphics_drawStringCentered(ctx, "** TIC TAC TOE **",
                                AUTO_STRING_LENGTH, 64, 25, OPAQUE_TEXT);

    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_WHITE);
    Graphics_drawStringCentered(ctx, "Move: Joystick",
                                AUTO_STRING_LENGTH, 64, 55, OPAQUE_TEXT);
    Graphics_drawStringCentered(ctx, "Place: Push stick",
                                AUTO_STRING_LENGTH, 64, 68, OPAQUE_TEXT);
    Graphics_drawStringCentered(ctx, "Switch: S1 button",
                                AUTO_STRING_LENGTH, 64, 81, OPAQUE_TEXT);
    Graphics_drawStringCentered(ctx, "Menu:   S2 button",
                                AUTO_STRING_LENGTH, 64, 94, OPAQUE_TEXT);

    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_YELLOW);
    Graphics_drawStringCentered(ctx, "Press S1 to start",
                                AUTO_STRING_LENGTH, 64, 114, OPAQUE_TEXT);

    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_WHITE);
}

//=== Main Menu ===

void Menu(Graphics_Context *ctx, int selection)
{
    Graphics_clearDisplay(ctx);

    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_ORANGE);
    Graphics_drawStringCentered(ctx, "-- Game Mode --",
                                AUTO_STRING_LENGTH, 64, 12, OPAQUE_TEXT);

    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_WHITE);
    Graphics_drawStringCentered(ctx, "Player vs Player",
                                AUTO_STRING_LENGTH, 64, 45, OPAQUE_TEXT);
    Graphics_drawStringCentered(ctx, "Player vs CPU",
                                AUTO_STRING_LENGTH, 64, 75, OPAQUE_TEXT);
    Graphics_drawStringCentered(ctx, "Two Games",
                                AUTO_STRING_LENGTH, 64, 105, OPAQUE_TEXT);

    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_LIME_GREEN);
    if(selection == 0)
        Graphics_drawString(ctx, "->", AUTO_STRING_LENGTH, 4, 42, OPAQUE_TEXT);
    else if(selection == 1)
        Graphics_drawString(ctx, "->", AUTO_STRING_LENGTH, 4, 72, OPAQUE_TEXT);
    else if(selection == 2)
        Graphics_drawString(ctx, "->", AUTO_STRING_LENGTH, 4, 102, OPAQUE_TEXT);

    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_WHITE);
}

//=== Dual Game Sub-Menu ===

void Mult_game(Graphics_Context *ctx, int selection)
{
    Graphics_clearDisplay(ctx);

    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_ORANGE);
    Graphics_drawStringCentered(ctx, "-- Two Games --",
                                AUTO_STRING_LENGTH, 64, 12, OPAQUE_TEXT);

    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_WHITE);
    Graphics_drawStringCentered(ctx, "Both vs Player",
                                AUTO_STRING_LENGTH, 64, 45, OPAQUE_TEXT);
    Graphics_drawStringCentered(ctx, "Both vs CPU",
                                AUTO_STRING_LENGTH, 64, 75, OPAQUE_TEXT);
    Graphics_drawStringCentered(ctx, "One each mode",
                                AUTO_STRING_LENGTH, 64, 105, OPAQUE_TEXT);

    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_LIME_GREEN);
    if(selection == 0)
        Graphics_drawString(ctx, "->", AUTO_STRING_LENGTH, 4, 42, OPAQUE_TEXT);
    else if(selection == 1)
        Graphics_drawString(ctx, "->", AUTO_STRING_LENGTH, 4, 72, OPAQUE_TEXT);
    else if(selection == 2)
        Graphics_drawString(ctx, "->", AUTO_STRING_LENGTH, 4, 102, OPAQUE_TEXT);

    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_WHITE);
}

//=== Board ===

void drawBoard(Graphics_Context *ctx)
{
    Graphics_clearDisplay(ctx);

    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_WHITE);

    // Two vertical dividers
    Graphics_drawLine(ctx, GRID_LEFT + CELL_SIZE, GRID_TOP,
                           GRID_LEFT + CELL_SIZE, GRID_BOTTOM);
    Graphics_drawLine(ctx, GRID_LEFT + CELL_SIZE * 2, GRID_TOP,
                           GRID_LEFT + CELL_SIZE * 2, GRID_BOTTOM);

    // Two horizontal dividers
    Graphics_drawLine(ctx, GRID_LEFT, GRID_TOP + CELL_SIZE,
                           GRID_RIGHT, GRID_TOP + CELL_SIZE);
    Graphics_drawLine(ctx, GRID_LEFT, GRID_TOP + CELL_SIZE * 2,
                           GRID_RIGHT, GRID_TOP + CELL_SIZE * 2);
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
    {
        for(c = 0; c < 3; c++)
        {
            if(grid[r][c] == 'X') drawX(ctx, r, c);
            if(grid[r][c] == 'O') drawO(ctx, r, c);
        }
    }
}

void drawX(Graphics_Context *ctx, int r, int c)
{
    int x0 = GRID_LEFT + (c * CELL_SIZE) + (CELL_SIZE - MARK_SIZE) / 2;
    int y0 = GRID_TOP  + (r * CELL_SIZE) + (CELL_SIZE - MARK_SIZE) / 2;

    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_DODGER_BLUE);
    Graphics_drawLine(ctx, x0,            y0,
                           x0 + MARK_SIZE, y0 + MARK_SIZE);
    Graphics_drawLine(ctx, x0 + MARK_SIZE, y0,
                           x0,            y0 + MARK_SIZE);
    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_WHITE);
}

void drawO(Graphics_Context *ctx, int r, int c)
{
    int cx = GRID_LEFT + (c * CELL_SIZE) + CELL_SIZE / 2;
    int cy = GRID_TOP  + (r * CELL_SIZE) + CELL_SIZE / 2;

    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_HOT_PINK);
    Graphics_drawCircle(ctx, cx, cy, CIRCLE_R);
    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_WHITE);
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

    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_YELLOW);
    Graphics_drawRectangle(ctx, &box);
    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_WHITE);
}

void clearCell(Graphics_Context *ctx, int r, int c)
{
    int x = GRID_LEFT + (c * CELL_SIZE) + 3;
    int y = GRID_TOP  + (r * CELL_SIZE) + 3;

    Graphics_Rectangle box;
    box.xMin = x;
    box.xMax = x + CELL_SIZE - 6;
    box.yMin = y;
    box.yMax = y + CELL_SIZE - 6;

    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_BLACK);
    Graphics_fillRectangle(ctx, &box);
    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_WHITE);
}

//=== Game Info ===

void showTurn(Graphics_Context *ctx, char player)
{
    char msg[12];
    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_DARK_ORANGE);
    if(player == 'X')
        Graphics_drawStringCentered(ctx, "Your turn: X",
                                    AUTO_STRING_LENGTH, 64, 122, OPAQUE_TEXT);
    else
        Graphics_drawStringCentered(ctx, "Your turn: O",
                                    AUTO_STRING_LENGTH, 64, 122, OPAQUE_TEXT);
    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_WHITE);
}

void showGameNumber(Graphics_Context *ctx, int num)
{
    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_DARK_ORANGE);
    if(num == 1)
        Graphics_drawStringCentered(ctx, "[ Game A ]",
                                    AUTO_STRING_LENGTH, 64, 4, OPAQUE_TEXT);
    else
        Graphics_drawStringCentered(ctx, "[ Game B ]",
                                    AUTO_STRING_LENGTH, 64, 4, OPAQUE_TEXT);
    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_WHITE);
}

//=== End Screens ===

void showWinner(Graphics_Context *ctx, char winner)
{
    Graphics_clearDisplay(ctx);
    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_LIME_GREEN);

    if(winner == 'X')
        Graphics_drawStringCentered(ctx, "X wins!",
                                    AUTO_STRING_LENGTH, 64, 50, OPAQUE_TEXT);
    else
        Graphics_drawStringCentered(ctx, "O wins!",
                                    AUTO_STRING_LENGTH, 64, 50, OPAQUE_TEXT);

    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_WHITE);
    Graphics_drawStringCentered(ctx, "Resetting...",
                                AUTO_STRING_LENGTH, 64, 75, OPAQUE_TEXT);
}

void showTie(Graphics_Context *ctx)
{
    Graphics_clearDisplay(ctx);
    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_YELLOW);
    Graphics_drawStringCentered(ctx, "It's a draw!",
                                AUTO_STRING_LENGTH, 64, 50, OPAQUE_TEXT);
    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_WHITE);
    Graphics_drawStringCentered(ctx, "Resetting...",
                                AUTO_STRING_LENGTH, 64, 75, OPAQUE_TEXT);
}

void showLoser(Graphics_Context *ctx)
{
    Graphics_clearDisplay(ctx);
    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_INDIAN_RED);
    Graphics_drawStringCentered(ctx, "CPU wins!",
                                AUTO_STRING_LENGTH, 64, 45, OPAQUE_TEXT);
    Graphics_setForegroundColor(ctx, GRAPHICS_COLOR_WHITE);
    Graphics_drawStringCentered(ctx, "Better luck",
                                AUTO_STRING_LENGTH, 64, 68, OPAQUE_TEXT);
    Graphics_drawStringCentered(ctx, "next time!",
                                AUTO_STRING_LENGTH, 64, 81, OPAQUE_TEXT);
}
