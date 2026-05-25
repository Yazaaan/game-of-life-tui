#ifndef ENGINE
#define ENGINE

#include <stdbool.h>

#define UNIVERSE_WIDTH 150
#define UNIVERSE_HEIGHT 50
#define ALIVE 1
#define DEAD 0

typedef struct{
  bool grid[UNIVERSE_HEIGHT][UNIVERSE_WIDTH];
  int width;
  int height;
} Universe;

Universe get_empty_universe();

void time_step(Universe* universe);

#endif // !ENGINE

