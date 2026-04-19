// game.c
// This file runs the actual game logic.
// It stores the board data, checks for wins and ties,
// handles player input, calls the AI for CPU moves,
// and loops through the game until someone wins or the player exits.

#include "msp430fr6989.h"
#include "game.h"
#include "display.h"
#include "joystick.h"
#include "sound.h"
#include "ai.h"

// =============================================================================
// Board Data - stored here and used by all game functions
// boardA and boardB are the two grids used in dual game mode
// Each cell holds EMPTY, PLAYER_X, or PLAYER_O
// =============================================================================
static char boardA[3][3];       // grid for Game A
static char boardB[3][3];       // grid for Game B
static char turnA, turnB;       // whose turn it is on each board
static int curRowA, curColA;    // cursor position for Game A
static int curRowB, curColB;    // cursor position for Game B
static int oldRow, oldCol;      // previous cursor position, used to detect movement

// these are declared volatile because they are changed inside the interrupt
volatile int activeBoard = 1;   // 1 = showing Game A, 2 = showing Game B
volatile int needsRedraw = 1;   // set to 1 whenever the screen needs to be redrawn
volatile int exitToMenu  = 0;   // set to 1 by S2 interrupt to exit game loop

// =============================================================================
// Internal Helper Functions
// These are only used inside this file (static)
// =============================================================================

// resets a board back to all empty and puts cursor at top-left
static void resetBoard(char board[3][3], char *turn, int *row, int *col)
{
    int r, c;
    for(r = 0; r < 3; r++)
        for(c = 0; c < 3; c++)
            board[r][c] = EMPTY;
    *turn = PLAYER_X;  // X always goes first
    *row  = 0;
    *col  = 0;
    needsRedraw = 1;
}

// checks all rows, columns, and diagonals for three in a row
// returns the winning symbol (X or O) or EMPTY if no winner yet
static char checkWin(char board[3][3])
{
    int i;
    for(i = 0; i < 3; i++)
    {
        // check each row
        if(board[i][0] != EMPTY && board[i][0] == board[i][1] && board[i][1] == board[i][2])
            return board[i][0];
        // check each column
        if(board[0][i] != EMPTY && board[0][i] == board[1][i] && board[1][i] == board[2][i])
            return board[0][i];
    }
    // check top-left to bottom-right diagonal
    if(board[0][0] != EMPTY && board[0][0] == board[1][1] && board[1][1] == board[2][2])
        return board[0][0];
    // check top-right to bottom-left diagonal
    if(board[0][2] != EMPTY && board[0][2] == board[1][1] && board[1][1] == board[2][0])
        return board[0][2];
    return EMPTY;  // no winner found
}

// returns 1 if every cell is filled (tie game), 0 if there are still empty spots
static int isBoardFull(char board[3][3])
{
    int r, c;
    for(r = 0; r < 3; r++)
        for(c = 0; c < 3; c++)
            if(board[r][c] == EMPTY) return 0;
    return 1;
}

// called after every move to check if the game ended
// shows the result screen, plays a sound, then resets the board
// returns 1 if game ended, 0 if still going
static int handleResult(Graphics_Context *ctx, char board[3][3],
                         int *row, int *col, char *turn)
{
    char winner = checkWin(board);
    if(winner != EMPTY)
    {
        if(winner == PLAYER_X) { showWinner(ctx, PLAYER_X); buzzer_win(); }
        else                   { showLoser(ctx);             buzzer_lose(); }
        __delay_cycles(10000000);  // pause so player can see the result screen
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

// =============================================================================
// Dual Mode Screen Drawing and Cursor Helpers
// Each game has its own draw and cursor function so they stay independent
// =============================================================================

// redraws Game A's board on screen - only runs if needsRedraw is set
static void drawGameAScreen(Graphics_Context *ctx)
{
    if(needsRedraw)
    {
        drawBoard(ctx);                         // clears screen and draws red grid lines
        drawMarks(ctx, boardA);                 // draws all X and O marks on boardA
        drawCursor(ctx, curRowA, curColA);      // draws gold cursor box
        showGameNumber(ctx, 1);                 // shows "Game A" at top
        showTurn(ctx, turnA);                   // shows whose turn it is at bottom
        needsRedraw = 0;
    }
}

// redraws Game B's board on screen - only runs if needsRedraw is set
static void drawGameBScreen(Graphics_Context *ctx)
{
    if(needsRedraw)
    {
        drawBoard(ctx);
        drawMarks(ctx, boardB);
        drawCursor(ctx, curRowB, curColB);
        showGameNumber(ctx, 2);                 // shows "Game B" at top
        showTurn(ctx, turnB);
        needsRedraw = 0;
    }
}

// moves Game A's cursor and flags a redraw if it moved
static void moveGameACursor(void)
{
    oldRow = curRowA;
    oldCol = curColA;
    movecursor(&curRowA, &curColA);  // reads joystick and updates row/col
    if(oldRow != curRowA || oldCol != curColA)
        needsRedraw = 1;  // cursor moved, so screen needs to update
}

// moves Game B's cursor and flags a redraw if it moved
static void moveGameBCursor(void)
{
    oldRow = curRowB;
    oldCol = curColB;
    movecursor(&curRowB, &curColB);
    if(oldRow != curRowB || oldCol != curColB)
        needsRedraw = 1;
}

// =============================================================================
// Single Game Modes
// =============================================================================

// Player vs Player - two players share the same joystick and take turns
// X always goes first, then they alternate until someone wins or it's a tie
void run_pvp(Graphics_Context *ctx)
{
    char board[3][3];
    char turn = PLAYER_X;
    int row = 0, col = 0, prevRow, prevCol, r, c;

    // start with empty board
    for(r = 0; r < 3; r++)
        for(c = 0; c < 3; c++)
            board[r][c] = EMPTY;

    needsRedraw = 1;
    exitToMenu  = 0;

    while(!exitToMenu)
    {
        // full redraw when something changes (piece placed, game reset, etc.)
        if(needsRedraw)
        {
            drawBoard(ctx);
            drawMarks(ctx, board);
            drawCursor(ctx, row, col);
            showTurn(ctx, turn);
            needsRedraw = 0;
        }

        // check if joystick moved and update cursor position
        prevRow = row; prevCol = col;
        movecursor(&row, &col);
        if(prevRow != row || prevCol != col)
            needsRedraw = 1;  // cursor moved, redraw next loop

        // joystick button pressed - try to place a mark
        if(readJoystick() == SELECT)
        {
            if(board[row][col] == EMPTY)
            {
                board[row][col] = turn;   // place current player's mark
                needsRedraw = 1;
                if(!handleResult(ctx, board, &row, &col, &turn))
                    turn = (turn == PLAYER_X) ? PLAYER_O : PLAYER_X;  // switch turns
            }
            else buzzer_error();          // cell already taken, play error sound
            checkBoosterS2Release();      // wait for button release so it doesn't repeat
        }
    }
}

// Player vs CPU - player is always X, computer plays as O
// after the player places X, the AI automatically picks a spot for O
void run_pvc(Graphics_Context *ctx)
{
    char board[3][3];
    char turn = PLAYER_X;
    int row = 0, col = 0, prevRow, prevCol, r, c;
    GameState gs;  // temporary struct used to pass board state to the AI

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
                board[row][col] = PLAYER_X;   // player places X

                if(!handleResult(ctx, board, &row, &col, &turn))
                {
                    // copy board into GameState so AI can read it
                    for(r=0;r<3;r++) for(c=0;c<3;c++) gs.board[r][c] = board[r][c];
                    gs.currentPlayer = PLAYER_O;
                    ai_move(&gs);              // AI picks its spot
                    // copy AI result back into board
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

// =============================================================================
// Dual Game Mode - two games run at the same time
// The screen shows one game at a time
// Press S1 to switch between Game A and Game B
// Each game keeps its own board, cursor, and turn separately
// =============================================================================

// initializes both boards before starting a dual game
void game_setup(void)
{
    resetBoard(boardA, &turnA, &curRowA, &curColA);
    resetBoard(boardB, &turnB, &curRowB, &curColB);
    activeBoard = 1;    // start on Game A
    needsRedraw = 1;
    exitToMenu  = 0;
}

// Two games both in Player vs Player mode
// activeBoard switches between 1 and 2 when S1 is pressed
void run_dual_pvp(Graphics_Context *ctx)
{
    game_setup();

    while(!exitToMenu)
    {
        if(activeBoard == 1)
        {
            // --- Game A ---
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
            // --- Game B ---
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

// Two games both in Player vs CPU mode
void run_dual_pvc(Graphics_Context *ctx)
{
    GameState gs;
    int r, c;
    game_setup();

    while(!exitToMenu)
    {
        if(activeBoard == 1)
        {
            // --- Game A - Player vs CPU ---
            drawGameAScreen(ctx);
            moveGameACursor();

            if(readJoystick() == SELECT)
            {
                if(boardA[curRowA][curColA] == EMPTY)
                {
                    boardA[curRowA][curColA] = PLAYER_X;  // player places X
                    if(!handleResult(ctx, boardA, &curRowA, &curColA, &turnA))
                    {
                        // let CPU pick its spot
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
            // --- Game B - Player vs CPU ---
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

// Mixed mode - Game A is Player vs CPU, Game B is Player vs Player
void run_dual_mixed(Graphics_Context *ctx)
{
    GameState gs;
    int r, c;
    game_setup();

    while(!exitToMenu)
    {
        if(activeBoard == 1)
        {
            // --- Game A - Player vs CPU ---
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
            // --- Game B - Player vs Player ---
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
