#ifndef PRINTS
#define PRINTS

#include "engine.h"

void print_headline();

void print_controls();

void print_stats(Game_State *game);

void print_dividers(void);

void print_universe(Universe *universe, int offset_y, int offset_x,
                      int margin_y, int margin_x);

void print_message(int pos_y, int pos_x, char msg[]);

#endif // !PRINTS
