#include "../include/engine.h"
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

// Speichern eiens Universums in eine Datei (Slots wählbar)
int save_grid(Universe *universe, int slot) {
  if (universe == NULL) {
    return 1;
  }

  char filename[32];
  sprintf(filename, "save_%d.universe", slot);
  FILE *file = fopen(filename, "w");

  if (file == NULL) {
    return 1;
  }

  fprintf(file, "%d %d\n", universe->height, universe->width);

  for (int y = 0; y < universe->height; y++) {
    for (int x = 0; x < universe->width; x++) {
      fprintf(file, "%d ", get_cell_state(universe, y, x));
    }
    fprintf(file, "\n");
  }
  fclose(file);
  return 0;
}

int load_grid(Universe **universe_ptr, int slot) {
  if (universe_ptr == NULL || *universe_ptr == NULL) {
    return 1;
  }

  char filename[32];
  sprintf(filename, "save_%d.universe", slot);
  FILE *file = fopen(filename, "r");

  if (file == NULL) {
    return 1;
  }

  int height, width;
  if (fscanf(file, "%d %d", &height, &width) != 2) {
    fclose(file);
    return 1;
  }

  // reset_universe(universe, height, width);
  resize_universe(universe_ptr, height, width);

  Universe *universe = *universe_ptr;

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int state;
      if (fscanf(file, "%d", &state) == 1) {
        change_cell(universe, y, x, state);
      }
    }
  }

  universe->variable_dimension = false;

  fclose(file);
  return 0;
}
