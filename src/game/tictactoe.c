
#include "tictactoe.h"

// Returns CELL_X or CELL_O if someone won, otherwise CELL_EMPTY.
static Cell winner_at(const GameState* s)
{
    // rows
    for (int r = 0; r < 3; r++) {
        if (s->board[r][0] != CELL_EMPTY &&
            s->board[r][0] == s->board[r][1] &&
            s->board[r][1] == s->board[r][2]) return s->board[r][0];
    }
    // cols
    for (int c = 0; c < 3; c++) {
        if (s->board[0][c] != CELL_EMPTY &&
            s->board[0][c] == s->board[1][c] &&
            s->board[1][c] == s->board[2][c]) return s->board[0][c];
    }
    // diags
    if (s->board[0][0] != CELL_EMPTY &&
        s->board[0][0] == s->board[1][1] &&
        s->board[1][1] == s->board[2][2]) return s->board[0][0];

    if (s->board[0][2] != CELL_EMPTY &&
        s->board[0][2] == s->board[1][1] &&
        s->board[1][1] == s->board[2][0]) return s->board[0][2];

    return CELL_EMPTY;
}

static uint8_t full(const GameState* s)
{
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            if (s->board[r][c] == CELL_EMPTY) return 0;
    return 1;
}

void TTT_Init(GameState* s)
{
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            s->board[r][c] = CELL_EMPTY;

    s->turn = CELL_X;
    s->status = STATUS_IN_PROGRESS;
}

uint8_t TTT_IsMoveValid(const GameState* s, uint8_t row, uint8_t col)
{
    if (row > 2 || col > 2) return 0;
    if (s->status != STATUS_IN_PROGRESS) return 0;
    return s->board[row][col] == CELL_EMPTY;
}

GameStatus TTT_Evaluate(GameState* s)
{
    Cell w = winner_at(s);
    if (w == CELL_X) { s->status = STATUS_X_WINS; return s->status; }
    if (w == CELL_O) { s->status = STATUS_O_WINS; return s->status; }
    if (full(s))     { s->status = STATUS_TIE;    return s->status; }

    s->status = STATUS_IN_PROGRESS;
    return s->status;
}

uint8_t TTT_ApplyMove(GameState* s, uint8_t row, uint8_t col)
{
    if (!TTT_IsMoveValid(s, row, col)) return 0;

    // Place the symbol for the current player.
    s->board[row][col] = s->turn;

    // Recompute status (win/tie/in-progress).
    TTT_Evaluate(s);

    // Only switch turns if game is still going.
    if (s->status == STATUS_IN_PROGRESS) {
        s->turn = (s->turn == CELL_X) ? CELL_O : CELL_X;
    }
    return 1;
}
