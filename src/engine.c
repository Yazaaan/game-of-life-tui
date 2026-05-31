#include "../include/engine.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define min(a, b) ((a) < (b) ? (a) : (b))

// Leeres Universum erzeugen in dem jede Zelle tot ist
Universe get_empty_universe(int height, int width) {
  Universe universe;
  universe.width = width;
  universe.height = height;
  universe.cells_alive = 0;
  universe.frameCount = 0;

  universe.grid = malloc(universe.height * sizeof(bool *));
  for (int y = 0; y < height; y++) {
    universe.grid[y] =
        calloc(width, sizeof(bool)); // calloc setzt alles auf false
  }
  return universe;
}

void fill_universe_random(Universe *universe) {
  srand(time(NULL));

  universe->frameCount = 0;

  for (int y = 0; y < universe->height; y++) {
    for (int x = 0; x < universe->width; x++) {
      int r = rand() % 3;
      int result = (r < 2) ? 0 : 1;
      change_cell(universe, y, x, result);
    }
  }
}

int count_Neighbours(Universe *universe, int y, int x) {
  int count = 0;

  for (int relative_y = -1; relative_y <= 1; relative_y++) {
    for (int relative_x = -1; relative_x <= 1; relative_x++) {
      if (relative_x == 0 && relative_y == 0)
        continue;

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

void change_cell(Universe *universe, int y, int x, bool new_state) {
  bool old_state = universe->grid[y][x];
  universe->grid[y][x] = new_state;

  // Nur wenn der neue Zustand vom alten Zustand unterschiedlich ist, soll
  // gezählt werden
  if (old_state != new_state) {
    if (new_state) {
      universe->cells_alive++;
    } else {
      universe->cells_alive--;
    }
  }
}

void time_step(Universe *universe) {
  Universe next_universe =
      get_empty_universe(universe->height, universe->width);

  next_universe.frameCount = universe->frameCount + 1;

  // Regeln durchsetzen
  for (int y = 0; y < universe->height; y++) {
    for (int x = 0; x < universe->width; x++) {
      int neighbours = count_Neighbours(universe, y, x);

      // Jede lebendigebdende zelle die ...
      if (universe->grid[y][x] == ALIVE) {
        // ... genau 2 oder 3 Nachbarn hat, überlebt
        if (neighbours == 2 || neighbours == 3) {
          change_cell(&next_universe, y, x, ALIVE);
          // next_universe.grid[y][x] = ALIVE;
        }
      }
      // Jede tote Zelle die ...
      else {
        // ... genau 3 Nachbarn hat wird lebendig
        if (neighbours == 3) {
          change_cell(&next_universe, y, x, ALIVE);
          // next_universe.grid[y][x] = ALIVE;
        }
      }
    }
  }

  destroy_universe(universe); // Altes Universum loswerden
  *universe = next_universe;  // Und durch die neue Iteration ersetzten
}

void resize_universe(Universe *universe, int new_height, int new_width) {
  int old_height = universe->height;
  int old_width = universe->width;
  bool **old_grid = universe->grid;

  // Neues leeres Universum mit neuer Größe erstellen
  *universe = get_empty_universe(new_height, new_width);

  // Bestimme die Grenzen für das Kopieren (immer das Kleinere)
  int copy_height = min(old_height, new_height);
  int copy_width = min(old_width, new_width);

  // Daten kopieren
  for (int y = 0; y < copy_height; y++) {
    for (int x = 0; x < copy_width; x++) {
      change_cell(universe, y, x, old_grid[y][x]);
    }
  }

  // Speicher freigeben
  for (int y = 0; y < old_height; y++) {
    free(old_grid[y]);
  }
  free(old_grid);
}

void destroy_universe(Universe *universe) {
  for (int i = 0; i < universe->height; i++) {
    free(universe->grid[i]);
  }
  free(universe->grid);
}
