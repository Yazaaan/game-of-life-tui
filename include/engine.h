#ifndef ENGINE
#define ENGINE

// Whether a cell is alive or dead should be represented by a bool. Although
// there is no bool in C, using one here improves the readability of the code.
#include <stdbool.h>

// To make things even clearer, two defines are introduced for “dead” and
// “alive”
#define ALIVE true
#define DEAD false

// This struct contains the game board with its cells as well as important
// properties, and is called “Universe” because it contains everything.
typedef struct
{
    bool *grid;
    int width;
    int height;
    bool variable_dimension;
    int cells_alive;
    long frame_count;
} Universe;

// This struct stores the game settings and contains its own universe
typedef struct
{
    Universe *universe;
    bool running;
    bool play;
    int simulation_speed;
    bool save_mode;
} Game_State;

bool get_cell_state(Universe *universe, int y, int x);

Universe *get_empty_universe(int height, int width, bool variable_dimension);

void reset_universe(Universe *universe);

void fill_universe_random(Universe *universe, int probability_percent);

int count_neighbours(Universe *universe, int y, int x);

void change_cell(Universe *universe, int y, int x, bool state);

void time_step(Universe *universe);

void resize_universe(Universe **old_universe, int new_height, int new_width);

void destroy_universe(Universe *universe);

#endif // !ENGINE
