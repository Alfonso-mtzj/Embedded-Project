
#include "render.h"
#include "../drivers/display.h"

// Layout constants
#define CELL_SIZE   38
#define LINE_THICK  2
#define OFF_X       7
#define OFF_Y       20

// Colors RGB565
#define C_BG     0x0000
#define C_GRID   0x39E7
#define C_CURSOR 0xFFE0
#define C_X      0xF800
#define C_O      0x001F
#define C_BAR    0x4208
#define C_TEXT   0xFFFF

static uint8_t cell_x(uint8_t col){ return OFF_X + col*(CELL_SIZE+LINE_THICK); }
static uint8_t cell_y(uint8_t row){ return OFF_Y + row*(CELL_SIZE+LINE_THICK); }

static void draw_grid(void)
{
    Display_FillRect(0, OFF_Y, 128, (CELL_SIZE*3 + LINE_THICK*2 + 2), C_BG);

    uint8_t v1 = OFF_X + CELL_SIZE;
    uint8_t v2 = v1 + LINE_THICK + CELL_SIZE;
    uint8_t top = OFF_Y;
    uint8_t bot = OFF_Y + CELL_SIZE*3 + LINE_THICK*2;

    Display_FillRect(v1, top, LINE_THICK, bot-top, C_GRID);
    Display_FillRect(v2, top, LINE_THICK, bot-top, C_GRID);

    uint8_t h1 = OFF_Y + CELL_SIZE;
    uint8_t h2 = h1 + LINE_THICK + CELL_SIZE;

    Display_FillRect(OFF_X, h1, CELL_SIZE*3 + LINE_THICK*2, LINE_THICK, C_GRID);
    Display_FillRect(OFF_X, h2, CELL_SIZE*3 + LINE_THICK*2, LINE_THICK, C_GRID);
}

static void draw_x(uint8_t r,uint8_t c)
{
    uint8_t x = cell_x(c)+5, y = cell_y(r)+5, sz = CELL_SIZE-10;
    for(uint8_t i=0;i<3;i++){
        Display_DrawLine(x+i,y,x+sz+i,y+sz,C_X);
        Display_DrawLine(x+i,y+sz,x+sz+i,y,C_X);
    }
}

static void draw_o(uint8_t r,uint8_t c)
{
    uint8_t cx = cell_x(c)+CELL_SIZE/2;
    uint8_t cy = cell_y(r)+CELL_SIZE/2;
    uint8_t ro = CELL_SIZE/2 - 5;
    uint8_t ri = ro - 4;

    for(int8_t dy=-(int8_t)ro; dy<=(int8_t)ro; dy++){
        for(int8_t dx=-(int8_t)ro; dx<=(int8_t)ro; dx++){
            int d2 = dx*dx + dy*dy;
            if(d2 <= ro*ro && d2 >= ri*ri)
                Display_FillRect(cx+dx, cy+dy, 1,1, C_O);
        }
    }
}

static void draw_piece(uint8_t r,uint8_t c, Cell p)
{
    Display_FillRect(cell_x(c), cell_y(r), CELL_SIZE, CELL_SIZE, C_BG);
    if(p==CELL_X) draw_x(r,c);
    else if(p==CELL_O) draw_o(r,c);
}

static void draw_cursor(uint8_t r,uint8_t c)
{
    uint8_t x = cell_x(c), y = cell_y(r);
    Display_FillRect(x,y,CELL_SIZE,2,C_CURSOR);
    Display_FillRect(x,y+CELL_SIZE-2,CELL_SIZE,2,C_CURSOR);
    Display_FillRect(x,y,2,CELL_SIZE,C_CURSOR);
    Display_FillRect(x+CELL_SIZE-2,y,2,CELL_SIZE,C_CURSOR);
}

void UI_Menu(uint8_t mode)
{
    Display_FillRect(0,0,128,128,C_BG);
    // Minimal “menu”: just color bar to indicate mode (no text)
    // mode=0 PvP, mode=1 PvC
    Display_FillRect(0,0,128,18, (mode?0x07FF:0xFD20)); // cyan for PvC, orange for PvP
}

void UI_RenderGame(const GameState* s, uint8_t cursor_row, uint8_t cursor_col, uint8_t game_index)
{
    // Status bar
    Display_FillRect(0,0,128,18,C_BAR);
    // Show A/B as colored pixel blocks (simple, no font required)
    Display_FillRect(2,4,10,10, game_index==0 ? 0xFFE0 : 0x07E0);

    draw_grid();

    for(uint8_t r=0;r<3;r++)
        for(uint8_t c=0;c<3;c++)
            draw_piece(r,c,s->board[r][c]);

    if(s->status == STATUS_IN_PROGRESS)
        draw_cursor(cursor_row,cursor_col);

    // Game over stripe
    if(s->status != STATUS_IN_PROGRESS)
        Display_FillRect(0,119,128,9,0x39E7);
}
