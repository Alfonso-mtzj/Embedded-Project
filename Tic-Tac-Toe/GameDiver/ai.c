// ai.c
// This file contains the computer player logic.
// The AI uses a simple strategy to pick the best move:
//   1. Win if it can
//   2. Block the player from winning
//   3. Take the center square
//   4. Take a corner
//   5. Take any open spot

#include "ai.h"

// =============================================================================
// Win Check Helper
// Tests whether placing sym at (r, c) would win the game
// Temporarily places the symbol, checks all win conditions, then removes it
// Returns 1 if it's a winning move, 0 if not
// =============================================================================
static int can_win(GameState *gs, int r, int c, char sym)
{
    if(gs->board[r][c] != EMPTY) return 0;  // cell already taken

    gs->board[r][c] = sym;  // try placing the symbol
    int won = 0;
    int i;

    for(i = 0; i < 3; i++)
    {
        // check row i
        if(gs->board[i][0]==sym && gs->board[i][1]==sym && gs->board[i][2]==sym) won=1;
        // check column i
        if(gs->board[0][i]==sym && gs->board[1][i]==sym && gs->board[2][i]==sym) won=1;
    }
    // check main diagonal
    if(gs->board[0][0]==sym && gs->board[1][1]==sym && gs->board[2][2]==sym) won=1;
    // check anti diagonal
    if(gs->board[0][2]==sym && gs->board[1][1]==sym && gs->board[2][0]==sym) won=1;

    gs->board[r][c] = EMPTY;  // undo the test move
    return won;
}

// =============================================================================
// AI Move Function
// Called after the player places X so the CPU can pick a spot for O
// Goes through priorities in order and returns as soon as it picks a move
// =============================================================================
void ai_move(GameState *gs)
{
    char ai  = gs->currentPlayer;                          // the CPU's symbol (O)
    char opp = (ai == PLAYER_X) ? PLAYER_O : PLAYER_X;   // the player's symbol (X)
    int r, c;

    // Priority 1: win the game if possible
    for(r=0;r<3;r++) for(c=0;c<3;c++)
        if(can_win(gs,r,c,ai)) { gs->board[r][c]=ai; return; }

    // Priority 2: block the player from winning on their next move
    for(r=0;r<3;r++) for(c=0;c<3;c++)
        if(can_win(gs,r,c,opp)) { gs->board[r][c]=ai; return; }

    // Priority 3: take the center square if it's open
    if(gs->board[1][1]==EMPTY) { gs->board[1][1]=ai; return; }

    // Priority 4: take any available corner
    // corners are stored as pairs of (row, col)
    int corners[4][2] = {{0,0},{0,2},{2,0},{2,2}};
    for(r=0;r<4;r++)
        if(gs->board[corners[r][0]][corners[r][1]]==EMPTY)
            { gs->board[corners[r][0]][corners[r][1]]=ai; return; }

    // Priority 5: take the first open spot found
    for(r=0;r<3;r++) for(c=0;c<3;c++)
        if(gs->board[r][c]==EMPTY) { gs->board[r][c]=ai; return; }
}
