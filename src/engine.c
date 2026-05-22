#include "../include/engine.h"
#include <stdbool.h>

int count_Neighbours(Universe *universe, int y, int x) {
  int count = 0;

  for (int relative_y = -1; relative_y <= 1; relative_y++) {
    for (int relative_x = -1; relative_x <= 1; relative_x++) {
      if(relative_x == 0 && relative_y == 0) continue;

      int check_y = y + relative_y;
      int check_x = x + relative_x;

      // Universumsgrenzen beachten
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
  // Regeln durchsetzen
  for (int y = 0; y < universe->height; y++) {
    for (int x = 0; x < universe->width; x++) {
      int neighbours = count_Neighbours(universe, y, x);

      // Jede Lebdende zelle die ...
      if (universe->grid[y][x] == true) {
        // ... weniger als 2 Nachbarn stribt
        if (neighbours < 2) {
          universe->grid[y][x] = false;
          // ... mehr als 3 Nachbarn stirbt
        } else if (neighbours > 3) {
          universe->grid[y][x] = false;
        }
      }
      // Jede tote Zelle die ...
      else {
        // ... genau 3 Nachbarn hat wird lebendig
        if (neighbours == 3) {
          universe->grid[y][x] = true;
        }
      }
    }
  }
}
