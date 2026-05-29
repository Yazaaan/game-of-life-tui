#include "../include/engine.h"
#define GRID_START_X 2
#define GRID_START_Y 5
#define SPEED_INCREMENT 25
#define MAX_SPEED 1000
#define MIN_SPEED 25

void ui_init(GameState *state);

void ui_cleanup();

void ui_input_process_keyboard(GameState *state);

void ui_draw(GameState *game);
