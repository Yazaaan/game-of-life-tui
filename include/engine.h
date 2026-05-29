#ifndef ENGINE
#define ENGINE

#include <stdbool.h>

#define UNIVERSE_WIDTH 150
#define UNIVERSE_HEIGHT 40
#define ALIVE 1
#define DEAD 0

typedef struct {
  bool grid[UNIVERSE_HEIGHT][UNIVERSE_WIDTH];
  int width;
  int height;
} Universe;

typedef struct {
  Universe universe;
  bool running;
  bool play;
  int simulationSpeed;
  long frameCount;
  char message[128];
} GameState;

Universe get_empty_universe();

void fill_universe_random(Universe *universe);

void time_step(Universe *universe);

#endif // !ENGINE
