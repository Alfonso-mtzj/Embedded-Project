// display.c
// This file handles everything that shows on the LCD screen.
// It draws the game board, the X and O marks, the cursor,
// the menus, and all the win/lose/tie screens.
// All pixel positions are calculated from the grid constants at the top.

#include "msp430fr6989.h"
#include "display.h"
#include <stdio.h>

// =============================================================================
// Grid Layout Constants
// The LCD is 128x128 pixels. The grid fits in a 108x108 area.
// Starting at x=10, y=10 with each cell being 36x36 pixels.
// The two divider lines create the 3x3 grid pattern.
// =============================================================================
#define GRID_LEFT   10    // left edge of the grid
#define GRID_TOP    10    // top edge of the grid
#define GRID_RIGHT  118   // right edge (10 + 3*36 = 118)
#define GRID_BOT    118   // bottom edge
#define CELL_SIZE   36    // each cell is 36 pixels wide and tall
#define DIV_X1      (GRID_LEFT + CELL_SIZE)      // first vertical line at x=46
#define DIV_X2      (GRID_LEFT + CELL_SIZE * 2)  // second vertical line at x=82
#define DIV_Y1      (GRID_TOP  + CELL_SIZE)      // first horizontal line at y=46
#define DIV_Y2      (GRID_TOP  + CELL_SIZE * 2)  // second horizontal line at y=82
#define MARK_SIZE   22    // how big the X marks are
#define CIRCLE_R    10    // radius of the O circles

// color shortcuts using a Mortal Kombat theme
#define MK_RED      GRAPHICS_COLOR_RED
#define MK_GOLD     GRAPHICS_COLOR_GOLD
#define MK_CYAN     GRAPHICS_COLOR_CYAN
#define MK_ORANGE   GRAPHICS_COLOR_ORANGE
#define MK_WHITE    GRAPHICS_COLOR_WHITE
#define MK_BLACK    GRAPHICS_COLOR_BLACK

// =============================================================================
// Clock and Display Initialization
// =============================================================================

// sets the DCO clock to 16 MHz so timing and SPI work correctly
// MCLK and SMCLK both run at 16 MHz after this
void Initialize_Clock_System(void)
{
    FRCTL0  = FRCTLPW | NWAITS_1;           // set FRAM wait state for 16 MHz
    CSCTL0  = CSKEY;                         // unlock clock registers
    CSCTL1 &= ~DCOFSEL_7;
    CSCTL1 |=  DCOFSEL_4 | DCORSEL;         // DCO = 16 MHz
    CSCTL3 &= ~(DIVS2 | DIVS1 | DIVS0);    // SMCLK divider = 1
    CSCTL3 &= ~(DIVM2 | DIVM1 | DIVM0);    // MCLK divider = 1
    CSCTL0_H = 0;                            // lock clock registers
}

// sets up the Crystalfontz 128x128 LCD with black background and white text
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

// =============================================================================
// Welcome and Menu Screens
// =============================================================================

// shows the title and control instructions when the board first powers on
// player presses S1 to move on to the main menu
void showWelcomeScreen(Graphics_Context *ctx)
{
    Graphics_clearDisplay(ctx);
    // title in red at the top
    Graphics_setForegroundColor(ctx, MK_RED);
    Graphics_drawStringCentered(ctx, "** TIC TAC TOE **",
                                AUTO_STRING_LENGTH, 64, 25, OPAQUE_TEXT);
    // control instructions in white
    Graphics_setForegroundColor(ctx, MK_WHITE);
    Graphics_drawStringCentered(ctx, "Move: Joystick",
                                AUTO_STRING_LENGTH, 64, 55, OPAQUE_TEXT);
    Graphics_drawStringCentered(ctx, "Place: Push stick",
                                AUTO_STRING_LENGTH, 64, 68, OPAQUE_TEXT);
    Graphics_drawStringCentered(ctx, "Switch: S1 button",
                                AUTO_STRING_LENGTH, 64, 81, OPAQUE_TEXT);
    Graphics_drawStringCentered(ctx, "Menu:   S2 button",
                                AUTO_STRING_LENGTH, 64, 94, OPAQUE_TEXT);
    // prompt to continue in gold at the bottom
    Graphics_setForegroundColor(ctx, MK_GOLD);
    Graphics_drawStringCentered(ctx, "Press S1 to start",
                                AUTO_STRING_LENGTH, 64, 114, OPAQUE_TEXT);
    Graphics_setForegroundColor(ctx, MK_WHITE);
}

// draws the main game mode menu
// the -> arrow points to whichever option is currently highlighted
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
    // gold arrow shows the selected option
    Graphics_setForegroundColor(ctx, MK_GOLD);
    if(selection == 0)
        Graphics_drawString(ctx, "->", AUTO_STRING_LENGTH, 4, 42, OPAQUE_TEXT);
    else if(selection == 1)
        Graphics_drawString(ctx, "->", AUTO_STRING_LENGTH, 4, 72, OPAQUE_TEXT);
    else if(selection == 2)
        Graphics_drawString(ctx, "->", AUTO_STRING_LENGTH, 4, 102, OPAQUE_TEXT);
    Graphics_setForegroundColor(ctx, MK_WHITE);
}

// draws the sub-menu for two game mode selection
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

// =============================================================================
// Board Drawing Functions
// All positions are calculated from GRID_LEFT, GRID_TOP, and CELL_SIZE
// so changing those constants moves the whole board
// =============================================================================

// clears the screen and draws the 4 red grid lines
void drawBoard(Graphics_Context *ctx)
{
    Graphics_clearDisplay(ctx);
    Graphics_setForegroundColor(ctx, MK_RED);
    Graphics_drawLine(ctx, DIV_X1, GRID_TOP, DIV_X1, GRID_BOT);   // left vertical
    Graphics_drawLine(ctx, DIV_X2, GRID_TOP, DIV_X2, GRID_BOT);   // right vertical
    Graphics_drawLine(ctx, GRID_LEFT, DIV_Y1, GRID_RIGHT, DIV_Y1); // top horizontal
    Graphics_drawLine(ctx, GRID_LEFT, DIV_Y2, GRID_RIGHT, DIV_Y2); // bottom horizontal
    Graphics_setForegroundColor(ctx, MK_WHITE);
}

// fills all cells in the array with a space character (clears board data)
void clearBoard(char grid[3][3])
{
    int r, c;
    for(r = 0; r < 3; r++)
        for(c = 0; c < 3; c++)
            grid[r][c] = ' ';
}

// loops through every cell and draws an X or O wherever there is a mark
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

// draws a cyan X in the given cell using two diagonal lines
void drawX(Graphics_Context *ctx, int r, int c)
{
    // calculate top-left corner of the mark inside the cell
    int x0 = GRID_LEFT + (c * CELL_SIZE) + (CELL_SIZE - MARK_SIZE) / 2;
    int y0 = GRID_TOP  + (r * CELL_SIZE) + (CELL_SIZE - MARK_SIZE) / 2;
    Graphics_setForegroundColor(ctx, MK_CYAN);
    Graphics_drawLine(ctx, x0,             y0,
                           x0 + MARK_SIZE, y0 + MARK_SIZE);  // top-left to bottom-right
    Graphics_drawLine(ctx, x0 + MARK_SIZE, y0,
                           x0,             y0 + MARK_SIZE);  // top-right to bottom-left
    Graphics_setForegroundColor(ctx, MK_WHITE);
}

// draws an orange circle O centered in the given cell
void drawO(Graphics_Context *ctx, int r, int c)
{
    int cx = GRID_LEFT + (c * CELL_SIZE) + CELL_SIZE / 2;  // center x
    int cy = GRID_TOP  + (r * CELL_SIZE) + CELL_SIZE / 2;  // center y
    Graphics_setForegroundColor(ctx, MK_ORANGE);
    Graphics_drawCircle(ctx, cx, cy, CIRCLE_R);
    Graphics_setForegroundColor(ctx, MK_WHITE);
}

// draws a gold rectangle around the cell to show where the cursor is
void drawCursor(Graphics_Context *ctx, int r, int c)
{
    int x = GRID_LEFT + (c * CELL_SIZE) + 2;  // slight inset from cell edge
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

// fills a cell with black to erase whatever was drawn there
// used before redrawing the cursor in a new position
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

// redraws the red grid lines that pass through a specific cell
// needed after clearCell erases them along with the cursor
void redrawCellLines(Graphics_Context *ctx, int r, int c)
{
    int x = GRID_LEFT + (c * CELL_SIZE);
    int y = GRID_TOP  + (r * CELL_SIZE);
    Graphics_setForegroundColor(ctx, MK_RED);
    // only draw lines that actually border this cell
    if(c > 0) Graphics_drawLine(ctx, x,             GRID_TOP, x,             GRID_BOT);
    if(c < 2) Graphics_drawLine(ctx, x + CELL_SIZE, GRID_TOP, x + CELL_SIZE, GRID_BOT);
    if(r > 0) Graphics_drawLine(ctx, GRID_LEFT, y,             GRID_RIGHT, y);
    if(r < 2) Graphics_drawLine(ctx, GRID_LEFT, y + CELL_SIZE, GRID_RIGHT, y + CELL_SIZE);
    Graphics_setForegroundColor(ctx, MK_WHITE);
}

// =============================================================================
// In-Game Status Display
// =============================================================================

// shows whose turn it is at the bottom of the screen (below the grid)
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

// shows "Game A" or "Game B" at the top of the screen in dual game mode
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

// =============================================================================
// End Game Screens
// =============================================================================

// shown when someone wins (used for PvP - shows X wins or O wins)
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

// shown when all 9 cells are filled with no winner
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

// shown when the CPU wins in player vs CPU mode
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
