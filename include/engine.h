#ifndef ENGINE
#define ENGINE

#include <stdbool.h>

#define UNIVERSE_WIDTH 150
#define UNIVERSE_HEIGHT 50

typedef struct{
  bool grid[UNIVERSE_HEIGHT][UNIVERSE_WIDTH];
  int width;
  int height;
} Universe;

void time_step(Universe* universe);

#endif // !ENGINE

