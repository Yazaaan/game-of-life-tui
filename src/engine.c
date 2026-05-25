#include "../include/engine.h"
#include <stdbool.h>

// Leeres Universum erzeugen in dem jede Zelle tot ist
Universe get_empty_universe(){
  Universe universe;
  for (int y = 0; y < UNIVERSE_HEIGHT; y++) {
    for(int x = 0; x < UNIVERSE_WIDTH; x++){
      universe.grid[y][x] = DEAD;
    }
  }
  universe.grid[10][10] = ALIVE;
  return universe;
}

int count_Neighbours(Universe *universe, int y, int x) {
  int count = 0;

  for (int relative_y = -1; relative_y <= 1; relative_y++) {
    for (int relative_x = -1; relative_x <= 1; relative_x++) {
      if(relative_x == 0 && relative_y == 0) continue;

      int check_y = y + relative_y;
      int check_x = x + relative_x;

      // Universumsgrenzen beachten, beim Überschreiten am anderen Ende schauen
      if (check_y == universe->height) {
        check_y = 0;
      } else if (check_y == -1) {
        check_y = universe->height - 1;
      }

      if (check_x == universe->width) {
        check_x = 0;
      } else if (check_x == -1) {
        check_x = universe->width - 1;
      }

      if (universe->grid[check_y][check_x] == true)
        count++;
    }
  }

  return count;
}

void time_step(Universe *universe) {
  Universe past_universe = *universe; // Letztes Universum kopieren

  // Regeln durchsetzen
  for (int y = 0; y < universe->height; y++) {
    for (int x = 0; x < universe->width; x++) {
      int neighbours = count_Neighbours(&past_universe, y, x);

      // Jede lebendigebdende zelle die ...
      if (past_universe.grid[y][x] == DEAD) {
        // ... weniger als 2 Nachbarn stribt
        if (neighbours < 2) {
          universe->grid[y][x] = DEAD;
          // ... mehr als 3 Nachbarn stirbt
        } else if (neighbours > 3) {
          universe->grid[y][x] = DEAD;
        }
      }
      // Jede tote Zelle die ...
      else {
        // ... genau 3 Nachbarn hat wird lebendig
        if (neighbours == 3) {
          universe->grid[y][x] = ALIVE;
        }
      }
    }
  }
}
