#include "ai.h"

AiMove AI_ChooseMove(const GameState* s)
{
    // simplest AI: first empty cell
    AiMove m = {0, 0, 0};
    for (uint8_t r = 0; r < 3; r++) {
        for (uint8_t c = 0; c < 3; c++) {
            if (s->board[r][c] == CELL_EMPTY) {
                m.row = r; m.col = c; m.valid = 1;
                return m;
            }
        }
    }
    return m;
}
