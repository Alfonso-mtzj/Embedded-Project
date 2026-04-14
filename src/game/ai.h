#ifndef AI_H
#define AI_H

#include <stdint.h>
#include "tictactoe.h"

typedef struct { uint8_t row, col, valid; } AiMove;

AiMove AI_ChooseMove(const GameState* s);

#endif
