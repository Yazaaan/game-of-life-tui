#ifndef ENGINE
#define ENGINE

#include <stdbool.h>

#define ALIVE 1
#define DEAD 0

typedef struct {
  bool **grid;
  int width;
  int height;
  int cells_alive;
} Universe;

typedef struct {
  Universe universe;
  bool running;
  bool play;
  int simulationSpeed;
  long frameCount;
  char message[128];
  bool variable_dimension;
} GameState;

Universe get_empty_universe(int height, int width);

void fill_universe_random(Universe *universe);

void change_cell(Universe *universe, int y, int x, bool state);

void time_step(Universe *universe);

void resize_universe(Universe* old_universe, int new_heigth, int new_width);

void destroy_universe(Universe *universe);

#endif // !ENGINE
