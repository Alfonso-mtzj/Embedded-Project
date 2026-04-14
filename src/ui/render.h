
#ifndef RENDER_H
#define RENDER_H

#include <stdint.h>
#include "../game/tictactoe.h"

void UI_Menu(uint8_t mode);
void UI_RenderGame(const GameState* s, uint8_t cursor_row, uint8_t cursor_col, uint8_t game_index);

#endif
