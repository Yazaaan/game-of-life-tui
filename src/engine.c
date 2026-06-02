#include "../include/engine.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define min(a, b) ((a) < (b) ? (a) : (b))

// Leeres Universum erzeugen in dem jede Zelle tot ist
Universe get_empty_universe(int height, int width, bool variable_dimension) {
  Universe universe;
  universe.width = width;
  universe.height = height;
  universe.variable_dimension = variable_dimension;
  universe.cells_alive = 0;
  universe.frame_count = 0;

  // Speicher für das neue Zell-Feld reservieren (Höhe * Speichergröße eines
  // Pointers)
  universe.grid = malloc(universe.height * sizeof(bool *));
  for (int y = 0; y < height; y++) {
    universe.grid[y] =
        calloc(width, sizeof(bool)); // calloc setzt alles auf false
  }
  return universe;
}

// Universum mit zufälligen Zuständen füllen
void fill_universe_random(Universe *universe, int probability_percent) {
  srand(time(NULL)); // Seed für den Zufallsgenerator auf Systemzeit setzen

  universe->frame_count = 0;

  for (int y = 0; y < universe->height; y++) {
    for (int x = 0; x < universe->width; x++) {
      int result = (rand() % 100 < probability_percent) ? ALIVE : DEAD;
      change_cell(universe, y, x, result);
    }
  }
}

// Zählt die Nachbarn um eine Zelle herum und gibt das Ergebnis zurück
int count_neighbours(Universe *universe, int y, int x) {
  int count = 0;

  for (int relative_y = -1; relative_y <= 1; relative_y++) {
    for (int relative_x = -1; relative_x <= 1; relative_x++) {
      if (relative_x == 0 && relative_y == 0)
        continue; // Soll sich nicht selbs zählen

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

      // Wenn Zelle an Prüfposition gefunden, counter erhöhen
      if (universe->grid[check_y][check_x] == true)
        count++;
    }
  }

  return count;
}

// Ändert den Zustand einer Zelle in einem Universum
void change_cell(Universe *universe, int y, int x, bool new_state) {
  bool old_state = universe->grid[y][x];
  universe->grid[y][x] = new_state;

  // Nur wenn der neue Zustand vom alten Zustand unterschiedlich ist und die
  // Zelle lebendig ist, soll gezählt werden
  if (old_state != new_state) {
    if (new_state) {
      universe->cells_alive++;
    } else {
      universe->cells_alive--;
    }
  }
}

// Hier wird eine Zeiteinheit in die Zukunft gegangen und der neue Zustand für
// jede Zelle auf Basis der Regeln des Spiels neu bestimmt
void time_step(Universe *universe) {
  // Es wird ein neues Universum für den neuen Frame angelegt, damit die
  // Berechnung der neuen Zellzustände auf dem alten Universum ausgeführt werden
  // können
  Universe next_universe =
      get_empty_universe(universe->height, universe->width, universe->variable_dimension);

  next_universe.frame_count = universe->frame_count + 1;

  // Regeln durchsetzen
  for (int y = 0; y < universe->height; y++) {
    for (int x = 0; x < universe->width; x++) {
      int neighbours = count_neighbours(universe, y, x);

      // Jede lebende Zelle die ...
      if (universe->grid[y][x] == ALIVE) {
        // ... genau 2 oder 3 Nachbarn hat, überlebt
        if (neighbours == 2 || neighbours == 3) {
          // Zustände werden in dem neuem Universum geändert
          change_cell(&next_universe, y, x, ALIVE);
        }
      }
      // Jede tote Zelle die ...
      else {
        // ... genau 3 Nachbarn hat wird lebendig
        if (neighbours == 3) {
          change_cell(&next_universe, y, x, ALIVE);
        }
      }
    }
  }

  destroy_universe(
      universe);             // Altes Universum loswerden und Speicher freigeben
  *universe = next_universe; // Und durch die neue Iteration ersetzten
}

// Diese Funktion kann die Größe eines Universums anpassen
void resize_universe(Universe *universe, int new_height, int new_width) {
  int old_height = universe->height;
  int old_width = universe->width;
  bool **old_grid = universe->grid;

  // Neues leeres Universum mit neuer Größe erstellen
  *universe = get_empty_universe(new_height, new_width, universe->variable_dimension);

  // Bestimmen der Grenzen für das Kopieren (immer das Kleinere)
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

// Speicher freigeben, wenn ein Universum nicht mehr gebraucht wird
void destroy_universe(Universe *universe) {
  for (int i = 0; i < universe->height; i++) {
    free(universe->grid[i]);
  }
  free(universe->grid);
}
