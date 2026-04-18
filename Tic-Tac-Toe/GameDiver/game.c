#include "msp430fr6989.h"
#include "game.h"
#include "display.h"
#include "joystick.h"
#include "sound.h"
#include "ai.h"

static char boardA[3][3];
static char boardB[3][3];
static char turnA, turnB;
static int curRowA, curColA;
static int curRowB, curColB;

volatile int activeBoard  = 1;
volatile int needsRedraw  = 1;
volatile int exitToMenu   = 0;

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

static void updateCursor(Graphics_Context *ctx, char board[3][3],
                          int prevRow, int prevCol, int newRow, int newCol)
{
    clearCell(ctx, prevRow, prevCol);
    redrawCellLines(ctx, prevRow, prevCol);
    if(board[prevRow][prevCol] == PLAYER_X) drawX(ctx, prevRow, prevCol);
    if(board[prevRow][prevCol] == PLAYER_O) drawO(ctx, prevRow, prevCol);
    drawCursor(ctx, newRow, newCol);
}

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
            updateCursor(ctx, board, prevRow, prevCol, row, col);

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
            updateCursor(ctx, board, prevRow, prevCol, row, col);

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

void game_setup(void)
{
    resetBoard(boardA, &turnA, &curRowA, &curColA);
    resetBoard(boardB, &turnB, &curRowB, &curColB);
    activeBoard = 1;
    needsRedraw = 1;
    exitToMenu  = 0;
}

static void drawDualScreen(Graphics_Context *ctx)
{
    // Select which board to show based on activeBoard
    char (*board)[3] = (activeBoard == 1) ? boardA : boardB;
    char   turn      = (activeBoard == 1) ? turnA  : turnB;
    int    row       = (activeBoard == 1) ? curRowA : curRowB;
    int    col       = (activeBoard == 1) ? curColA : curColB;

    drawBoard(ctx);
    drawMarks(ctx, board);
    drawCursor(ctx, row, col);
    showGameNumber(ctx, activeBoard);
    showTurn(ctx, turn);
    needsRedraw = 0;
}

static void run_dual(Graphics_Context *ctx, int mode)
{
    int prevRow, prevCol, r, c;
    GameState gs;
    game_setup();

    while(!exitToMenu)
    {
        // needsRedraw is set by S1 interrupt OR after a move
        if(needsRedraw)
        {
            drawDualScreen(ctx);
        }

        char (*board)[3] = (activeBoard == 1) ? boardA : boardB;
        char  *turn      = (activeBoard == 1) ? &turnA : &turnB;
        int   *row       = (activeBoard == 1) ? &curRowA : &curRowB;
        int   *col       = (activeBoard == 1) ? &curColA : &curColB;

        prevRow = *row; prevCol = *col;
        movecursor(row, col);
        if(*row != prevRow || *col != prevCol)
            updateCursor(ctx, board, prevRow, prevCol, *row, *col);

        if(readJoystick() == SELECT)
        {
            if(board[*row][*col] == EMPTY)
            {
                int isPC = (mode == 1) || (mode == 2 && activeBoard == 1);

                if(isPC)
                {
                    board[*row][*col] = PLAYER_X;
                    if(!handleResult(ctx, board, row, col, turn))
                    {
                        for(r=0;r<3;r++) for(c=0;c<3;c++) gs.board[r][c]=board[r][c];
                        gs.currentPlayer = PLAYER_O;
                        ai_move(&gs);
                        for(r=0;r<3;r++) for(c=0;c<3;c++) board[r][c]=gs.board[r][c];
                        handleResult(ctx, board, row, col, turn);
                    }
                }
                else
                {
                    board[*row][*col] = *turn;
                    if(!handleResult(ctx, board, row, col, turn))
                        *turn = (*turn == PLAYER_X) ? PLAYER_O : PLAYER_X;
                }
                needsRedraw = 1;
            }
            else buzzer_error();
            checkBoosterS2Release();
        }
    }
}

void run_dual_pvp(Graphics_Context *ctx)   { run_dual(ctx, 0); }
void run_dual_pvc(Graphics_Context *ctx)   { run_dual(ctx, 1); }
void run_dual_mixed(Graphics_Context *ctx) { run_dual(ctx, 2); }
