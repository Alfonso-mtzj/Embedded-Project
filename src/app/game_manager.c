
#include "game_manager.h"

#include "../drivers/joystick.h"
#include "../drivers/display.h"
#include "../audio/sfx.h"
#include "../ui/render.h"
#include "../game/ai.h"

static void clamp_cursor(GameManager* gm)
{
    if (gm->cursor_row > 2) gm->cursor_row = 2;
    if (gm->cursor_col > 2) gm->cursor_col = 2;
}

void GameManager_Init(GameManager* gm)
{
    gm->active_game = 0;
    gm->mode = MODE_PVP;
    gm->state = APP_MENU;

    gm->cursor_row = 1;
    gm->cursor_col = 1;

    TTT_Init(&gm->games[0]);
    TTT_Init(&gm->games[1]);

    UI_Menu(gm->mode);
}

static void handle_menu(GameManager* gm, JoyAction a)
{
    if (a == JOY_UP)   gm->mode = MODE_PVC;
    if (a == JOY_DOWN) gm->mode = MODE_PVP;

    UI_Menu(gm->mode);

    if (a == JOY_SELECT) {
        gm->state = APP_PLAYING;
        // reset both games when starting
        TTT_Init(&gm->games[0]);
        TTT_Init(&gm->games[1]);
        gm->active_game = 0;
        gm->cursor_row = 1;
        gm->cursor_col = 1;
        SFX_SwitchGame();
    }
}

static void handle_playing(GameManager* gm, JoyAction a)
{
    GameState* s = &gm->games[gm->active_game];

    // Switch between Game A/B
    if (a == JOY_SWITCH_GAME) {
        gm->active_game ^= 1;
        SFX_SwitchGame();
        return;
    }

    // Move cursor
    if (a == JOY_UP    && gm->cursor_row > 0) gm->cursor_row--;
    if (a == JOY_DOWN  && gm->cursor_row < 2) gm->cursor_row++;
    if (a == JOY_LEFT  && gm->cursor_col > 0) gm->cursor_col--;
    if (a == JOY_RIGHT && gm->cursor_col < 2) gm->cursor_col++;
    clamp_cursor(gm);

    // Place move
    if (a == JOY_SELECT && s->status == STATUS_IN_PROGRESS) {
        uint8_t ok = TTT_ApplyMove(s, gm->cursor_row, gm->cursor_col);

        if (!ok) {
            SFX_Error();
        } else {
            SFX_Move();

            // PvC: computer plays O
            if (gm->mode == MODE_PVC &&
                s->status == STATUS_IN_PROGRESS &&
                s->turn == CELL_O)
            {
                AiMove m = AI_ChooseMove(s);
                if (m.valid) {
                    (void)TTT_ApplyMove(s, m.row, m.col);
                }
            }

            if (s->status == STATUS_X_WINS || s->status == STATUS_O_WINS) SFX_Win();
            else if (s->status == STATUS_TIE) SFX_Tie();
        }
    }

    // Optional reset current game
    if (a == JOY_RESET_GAME) {
        TTT_Init(s);
        SFX_SwitchGame();
    }
}

void GameManager_Tick(GameManager* gm)
{
    JoyAction a = Joystick_ReadAction();

    if (gm->state == APP_MENU) {
        handle_menu(gm, a);
        return;
    }

    handle_playing(gm, a);

    // Draw active game
    const GameState* s = &gm->games[gm->active_game];
    UI_RenderGame(s, gm->cursor_row, gm->cursor_col, gm->active_game);
}
