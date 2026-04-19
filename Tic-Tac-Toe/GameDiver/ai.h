// ai.h
// Declares the AI move function used in player vs CPU mode.

#ifndef AI_H
#define AI_H

#include "game.h"

// picks the best available move for the CPU and places it on the board
void ai_move(GameState *gs);

#endif
