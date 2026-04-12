#ifndef AI_H
#define AI_H

#include <stdint.h>
#include "tictactoe.h"

typedef struct {
    uint8_t row;
    uint8_t col;
    uint8_t valid;
} AiMove;

// Simple AI: choose a valid cell for O (or whoever is computer).
AiMove AI_ChooseMove(const GameState* s);

#endif
