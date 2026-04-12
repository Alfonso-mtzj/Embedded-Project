
#ifndef TICTACTOE_H
#define TICTACTOE_H

#include <stdint.h>

typedef enum { CELL_EMPTY=0, CELL_X, CELL_O } Cell;
typedef enum { STATUS_IN_PROGRESS=0, STATUS_X_WINS, STATUS_O_WINS, STATUS_TIE } GameStatus;

typedef struct {
    Cell board[3][3];
    Cell turn;          // CELL_X or CELL_O
    GameStatus status;  // in progress / win / tie
} GameState;

void TTT_Init(GameState* s);
uint8_t TTT_IsMoveValid(const GameState* s, uint8_t row, uint8_t col);
uint8_t TTT_ApplyMove(GameState* s, uint8_t row, uint8_t col);
GameStatus TTT_Evaluate(GameState* s);

#endif
