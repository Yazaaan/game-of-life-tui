#include "../include/engine.h"
#define GRID_START_X 28
#define GRID_START_Y 4
#define GRID_MARGIN_X 18
#define GRID_MARGIN_Y 2
#define TERMINAL_MIN_WIDTH 70
#define TERMINAL_MIN_HEIGHT 70
#define SPEED_INCREMENT 25
#define MAX_SPEED 1000
#define MIN_SPEED 25

void ui_init(Game_State *state);

void ui_cleanup();

void ui_process_input(Game_State* game);

void ui_draw(Game_State *game);
