// game.c
// This file controls the game logic.
// It handles player moves, checks for wins and ties,
// and runs the game loops for all three game modes.

#include "msp430fr6989.h"
#include "game.h"
#include "display.h"
#include "joystick.h"
#include "sound.h"
#include "ai.h"

// =====================================================================
// Board Storage - two boards for dual game mode
// =====================================================================
static char boardA[3][3];
static char boardB[3][3];
static char turnA, turnB;
static int curRowA, curColA;
static int curRowB, curColB;
static int oldRow, oldCol;   // used to detect cursor movement

// shared flags used by the interrupt in main.c
volatile int activeBoard  = 1;  // 1 = Game A, 2 = Game B
volatile int needsRedraw  = 1;  // 1 = screen needs to be redrawn
volatile int exitToMenu   = 0;  // 1 = go back to the main menu

// =====================================================================
// Helper Functions
// =====================================================================

static void resetBoard(char board[3][3], char *turn, int *row, int *col)
{
    int r, c;
    for(r = 0; r < 3; r++)
        for(c = 0; c < 3; c++)
            board[r][c] = EMPTY;
    *turn = PLAYER_X;
    *row  = 0;
    *col  = 0;
    needsRedraw = 1;
}

static char checkWin(char board[3][3])
{
    int i;
    for(i = 0; i < 3; i++)
    {
        if(board[i][0] != EMPTY && board[i][0] == board[i][1] && board[i][1] == board[i][2])
            return board[i][0];
        if(board[0][i] != EMPTY && board[0][i] == board[1][i] && board[1][i] == board[2][i])
            return board[0][i];
    }
    if(board[0][0] != EMPTY && board[0][0] == board[1][1] && board[1][1] == board[2][2])
        return board[0][0];
    if(board[0][2] != EMPTY && board[0][2] == board[1][1] && board[1][1] == board[2][0])
        return board[0][2];
    return EMPTY;
}

static int isBoardFull(char board[3][3])
{
    int r, c;
    for(r = 0; r < 3; r++)
        for(c = 0; c < 3; c++)
            if(board[r][c] == EMPTY) return 0;
    return 1;
}

static int handleResult(Graphics_Context *ctx, char board[3][3],
                         int *row, int *col, char *turn)
{
    char winner = checkWin(board);
    if(winner != EMPTY)
    {
        if(winner == PLAYER_X) { showWinner(ctx, PLAYER_X); buzzer_win(); }
        else                   { showLoser(ctx);             buzzer_lose(); }
        __delay_cycles(10000000);
        resetBoard(board, turn, row, col);
        return 1;
    }
    if(isBoardFull(board))
    {
        showTie(ctx); buzzer_tie();
        __delay_cycles(10000000);
        resetBoard(board, turn, row, col);
        return 1;
    }
    return 0;
}

// =====================================================================
// Screen draw helpers for dual mode
// =====================================================================

// draws Game A board - only redraws if needsRedraw is set
static void drawGameAScreen(Graphics_Context *ctx)
{
    if(needsRedraw)
    {
        drawBoard(ctx);
        drawMarks(ctx, boardA);
        drawCursor(ctx, curRowA, curColA);
        showGameNumber(ctx, 1);
        showTurn(ctx, turnA);
        needsRedraw = 0;
    }
}

// draws Game B board - only redraws if needsRedraw is set
static void drawGameBScreen(Graphics_Context *ctx)
{
    if(needsRedraw)
    {
        drawBoard(ctx);
        drawMarks(ctx, boardB);
        drawCursor(ctx, curRowB, curColB);
        showGameNumber(ctx, 2);
        showTurn(ctx, turnB);
        needsRedraw = 0;
    }
}

// moves cursor for Game A and sets redraw if it moved
static void moveGameACursor(void)
{
    oldRow = curRowA;
    oldCol = curColA;
    movecursor(&curRowA, &curColA);
    if(oldRow != curRowA || oldCol != curColA)
        needsRedraw = 1;
}

// moves cursor for Game B and sets redraw if it moved
static void moveGameBCursor(void)
{
    oldRow = curRowB;
    oldCol = curColB;
    movecursor(&curRowB, &curColB);
    if(oldRow != curRowB || oldCol != curColB)
        needsRedraw = 1;
}

// =====================================================================
// Single Game Modes
// =====================================================================

void run_pvp(Graphics_Context *ctx)
{
    char board[3][3];
    char turn = PLAYER_X;
    int row = 0, col = 0, prevRow, prevCol, r, c;

    for(r = 0; r < 3; r++)
        for(c = 0; c < 3; c++)
            board[r][c] = EMPTY;

    needsRedraw = 1;
    exitToMenu  = 0;

    while(!exitToMenu)
    {
        if(needsRedraw)
        {
            drawBoard(ctx);
            drawMarks(ctx, board);
            drawCursor(ctx, row, col);
            showTurn(ctx, turn);
            needsRedraw = 0;
        }

        prevRow = row; prevCol = col;
        movecursor(&row, &col);
        if(prevRow != row || prevCol != col)
            needsRedraw = 1;

        if(readJoystick() == SELECT)
        {
            if(board[row][col] == EMPTY)
            {
                board[row][col] = turn;
                needsRedraw = 1;
                if(!handleResult(ctx, board, &row, &col, &turn))
                    turn = (turn == PLAYER_X) ? PLAYER_O : PLAYER_X;
            }
            else buzzer_error();
            checkBoosterS2Release();
        }
    }
}

void run_pvc(Graphics_Context *ctx)
{
    char board[3][3];
    char turn = PLAYER_X;
    int row = 0, col = 0, prevRow, prevCol, r, c;
    GameState gs;

    for(r = 0; r < 3; r++)
        for(c = 0; c < 3; c++)
            board[r][c] = EMPTY;

    needsRedraw = 1;
    exitToMenu  = 0;

    while(!exitToMenu)
    {
        if(needsRedraw)
        {
            drawBoard(ctx);
            drawMarks(ctx, board);
            drawCursor(ctx, row, col);
            showTurn(ctx, turn);
            needsRedraw = 0;
        }

        prevRow = row; prevCol = col;
        movecursor(&row, &col);
        if(prevRow != row || prevCol != col)
            needsRedraw = 1;

        if(readJoystick() == SELECT)
        {
            if(board[row][col] == EMPTY)
            {
                board[row][col] = PLAYER_X;
                if(!handleResult(ctx, board, &row, &col, &turn))
                {
                    for(r=0;r<3;r++) for(c=0;c<3;c++) gs.board[r][c] = board[r][c];
                    gs.currentPlayer = PLAYER_O;
                    ai_move(&gs);
                    for(r=0;r<3;r++) for(c=0;c<3;c++) board[r][c] = gs.board[r][c];
                    handleResult(ctx, board, &row, &col, &turn);
                }
                needsRedraw = 1;
            }
            else buzzer_error();
            checkBoosterS2Release();
        }
    }
}

// =====================================================================
// Dual Game Mode
// Uses the same if/else pattern as friend's code:
// if activeBoard == 1 run Game A code, else run Game B code
// S1 interrupt toggles activeBoard and sets needsRedraw
// =====================================================================

void game_setup(void)
{
    resetBoard(boardA, &turnA, &curRowA, &curColA);
    resetBoard(boardB, &turnB, &curRowB, &curColB);
    activeBoard = 1;
    needsRedraw = 1;
    exitToMenu  = 0;
}

// PvP dual - both boards are player vs player
void run_dual_pvp(Graphics_Context *ctx)
{
    game_setup();

    while(!exitToMenu)
    {
        if(activeBoard == 1)
        {
            // Game A
            drawGameAScreen(ctx);
            moveGameACursor();

            if(readJoystick() == SELECT)
            {
                if(boardA[curRowA][curColA] == EMPTY)
                {
                    boardA[curRowA][curColA] = turnA;
                    if(!handleResult(ctx, boardA, &curRowA, &curColA, &turnA))
                        turnA = (turnA == PLAYER_X) ? PLAYER_O : PLAYER_X;
                    needsRedraw = 1;
                }
                else buzzer_error();
                checkBoosterS2Release();
            }
        }
        else
        {
            // Game B
            drawGameBScreen(ctx);
            moveGameBCursor();

            if(readJoystick() == SELECT)
            {
                if(boardB[curRowB][curColB] == EMPTY)
                {
                    boardB[curRowB][curColB] = turnB;
                    if(!handleResult(ctx, boardB, &curRowB, &curColB, &turnB))
                        turnB = (turnB == PLAYER_X) ? PLAYER_O : PLAYER_X;
                    needsRedraw = 1;
                }
                else buzzer_error();
                checkBoosterS2Release();
            }
        }
    }
}

// PvC dual - both boards are player vs CPU
void run_dual_pvc(Graphics_Context *ctx)
{
    GameState gs;
    int r, c;
    game_setup();

    while(!exitToMenu)
    {
        if(activeBoard == 1)
        {
            // Game A - player vs CPU
            drawGameAScreen(ctx);
            moveGameACursor();

            if(readJoystick() == SELECT)
            {
                if(boardA[curRowA][curColA] == EMPTY)
                {
                    boardA[curRowA][curColA] = PLAYER_X;
                    if(!handleResult(ctx, boardA, &curRowA, &curColA, &turnA))
                    {
                        for(r=0;r<3;r++) for(c=0;c<3;c++) gs.board[r][c]=boardA[r][c];
                        gs.currentPlayer = PLAYER_O;
                        ai_move(&gs);
                        for(r=0;r<3;r++) for(c=0;c<3;c++) boardA[r][c]=gs.board[r][c];
                        handleResult(ctx, boardA, &curRowA, &curColA, &turnA);
                    }
                    needsRedraw = 1;
                }
                else buzzer_error();
                checkBoosterS2Release();
            }
        }
        else
        {
            // Game B - player vs CPU
            drawGameBScreen(ctx);
            moveGameBCursor();

            if(readJoystick() == SELECT)
            {
                if(boardB[curRowB][curColB] == EMPTY)
                {
                    boardB[curRowB][curColB] = PLAYER_X;
                    if(!handleResult(ctx, boardB, &curRowB, &curColB, &turnB))
                    {
                        for(r=0;r<3;r++) for(c=0;c<3;c++) gs.board[r][c]=boardB[r][c];
                        gs.currentPlayer = PLAYER_O;
                        ai_move(&gs);
                        for(r=0;r<3;r++) for(c=0;c<3;c++) boardB[r][c]=gs.board[r][c];
                        handleResult(ctx, boardB, &curRowB, &curColB, &turnB);
                    }
                    needsRedraw = 1;
                }
                else buzzer_error();
                checkBoosterS2Release();
            }
        }
    }
}

// Mixed dual - Game A is player vs CPU, Game B is player vs player
void run_dual_mixed(Graphics_Context *ctx)
{
    GameState gs;
    int r, c;
    game_setup();

    while(!exitToMenu)
    {
        if(activeBoard == 1)
        {
            // Game A - player vs CPU
            drawGameAScreen(ctx);
            moveGameACursor();

            if(readJoystick() == SELECT)
            {
                if(boardA[curRowA][curColA] == EMPTY)
                {
                    boardA[curRowA][curColA] = PLAYER_X;
                    if(!handleResult(ctx, boardA, &curRowA, &curColA, &turnA))
                    {
                        for(r=0;r<3;r++) for(c=0;c<3;c++) gs.board[r][c]=boardA[r][c];
                        gs.currentPlayer = PLAYER_O;
                        ai_move(&gs);
                        for(r=0;r<3;r++) for(c=0;c<3;c++) boardA[r][c]=gs.board[r][c];
                        handleResult(ctx, boardA, &curRowA, &curColA, &turnA);
                    }
                    needsRedraw = 1;
                }
                else buzzer_error();
                checkBoosterS2Release();
            }
        }
        else
        {
            // Game B - player vs player
            drawGameBScreen(ctx);
            moveGameBCursor();

            if(readJoystick() == SELECT)
            {
                if(boardB[curRowB][curColB] == EMPTY)
                {
                    boardB[curRowB][curColB] = turnB;
                    if(!handleResult(ctx, boardB, &curRowB, &curColB, &turnB))
                        turnB = (turnB == PLAYER_X) ? PLAYER_O : PLAYER_X;
                    needsRedraw = 1;
                }
                else buzzer_error();
                checkBoosterS2Release();
            }
        }
    }
}
