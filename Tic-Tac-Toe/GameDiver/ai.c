#include "ai.h"

static int can_win(GameState *gs, int r, int c, char sym)
{
    if(gs->board[r][c] != EMPTY) return 0;
    gs->board[r][c] = sym;
    int won = 0;
    int i;
    for(i = 0; i < 3; i++)
    {
        if(gs->board[i][0]==sym && gs->board[i][1]==sym && gs->board[i][2]==sym) won=1;
        if(gs->board[0][i]==sym && gs->board[1][i]==sym && gs->board[2][i]==sym) won=1;
    }
    if(gs->board[0][0]==sym && gs->board[1][1]==sym && gs->board[2][2]==sym) won=1;
    if(gs->board[0][2]==sym && gs->board[1][1]==sym && gs->board[2][0]==sym) won=1;
    gs->board[r][c] = EMPTY;
    return won;
}

void ai_move(GameState *gs)
{
    char ai  = gs->currentPlayer;
    char opp = (ai == PLAYER_X) ? PLAYER_O : PLAYER_X;
    int r, c;

    // Win if possible
    for(r=0;r<3;r++) for(c=0;c<3;c++)
        if(can_win(gs,r,c,ai)) { gs->board[r][c]=ai; return; }

    // Block opponent
    for(r=0;r<3;r++) for(c=0;c<3;c++)
        if(can_win(gs,r,c,opp)) { gs->board[r][c]=ai; return; }

    // Take center
    if(gs->board[1][1]==EMPTY) { gs->board[1][1]=ai; return; }

    // Take a corner
    int corners[4][2] = {{0,0},{0,2},{2,0},{2,2}};
    for(r=0;r<4;r++)
        if(gs->board[corners[r][0]][corners[r][1]]==EMPTY)
            { gs->board[corners[r][0]][corners[r][1]]=ai; return; }

    // Take any open spot
    for(r=0;r<3;r++) for(c=0;c<3;c++)
        if(gs->board[r][c]==EMPTY) { gs->board[r][c]=ai; return; }
}
