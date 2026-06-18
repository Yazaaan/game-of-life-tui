#include "../include/engine.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define min(a, b) ((a) < (b) ? (a) : (b))

inline bool get_cell_state(Universe *universe, int y, int x) {
  if ((unsigned int)y >= universe->height || (unsigned int)x >= universe->width)
    return DEAD;
  return universe->grid[y * universe->width + x];
}

// Ändert den Zustand einer Zelle in einem Universum
void change_cell(Universe *universe, int y, int x, bool new_state) {
  if (y >= universe->height || x >= universe->width || y < 0 || x < 0)
    exit(EXIT_FAILURE);

  bool state = get_cell_state(universe, y, x);
  universe->grid[y * universe->width + x] = new_state;

  // Nur wenn der neue Zustand vom alten Zustand unterschiedlich ist und die
  // Zelle lebendig ist, soll gezählt werden
  if (state != new_state) {
    if (new_state) {
      universe->cells_alive++;
    } else {
      universe->cells_alive--;
    }
  }
}

// Leeres Universum erzeugen in dem jede Zelle tot ist
Universe *get_empty_universe(int height, int width, bool variable_dimension) {
  Universe *universe = malloc(sizeof(Universe));
  if (universe == NULL) {
    exit(EXIT_FAILURE);
  }

  universe->width = width;
  universe->height = height;
  universe->variable_dimension = variable_dimension;
  universe->cells_alive = 0;
  universe->frame_count = 0;

  // Speicher für das neue Zell-Feld reservieren (Höhe * Breite * Speichergröße)
  universe->grid = (bool *)calloc(height * width, sizeof(bool));
  if (universe->grid == NULL) {
    free(universe);
    exit(EXIT_FAILURE);
  }

  return universe;
}

// Eigene Funktion um ein Universum zu leeren
void reset_universe(Universe *universe) {
  if (universe == NULL)
    exit(EXIT_FAILURE);

  memset(universe->grid, DEAD,
         universe->height * universe->width *
             sizeof(bool)); // Füllt alles ab der Adresse vom Grid bis zu
                            // seinem Ende mit Nullen

  universe->cells_alive = 0;
  universe->frame_count = 0;
}

// Universum mit zufälligen Zuständen füllen
void fill_universe_random(Universe *universe, int probability_percent) {
  if (universe == NULL)
    exit(EXIT_FAILURE);

  srand(time(NULL) +
        universe->cells_alive); // Seed für den Zufallsgenerator auf Systemzeit
                                // + Anzahl Zellen aus altem Universum setzen

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
  if (universe == NULL)
    return 0;

  int count = 0;

  for (int relative_y = -1; relative_y <= 1; relative_y++) {
    for (int relative_x = -1; relative_x <= 1; relative_x++) {
      if (relative_x == 0 && relative_y == 0)
        continue; // Soll sich nicht selbst zählen

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
      if (get_cell_state(universe, check_y, check_x) == true)
        count++;
    }
  }

  return count;
}

// Hier wird eine Zeiteinheit in die Zukunft gegangen und der neue Zustand für
// jede Zelle auf Basis der Regeln des Spiels neu bestimmt
void time_step(Universe *universe) {
  if (universe == NULL)
    exit(EXIT_FAILURE);

  // Es wird ein neues Universum für den neuen Frame angelegt, damit die
  // Berechnung der neuen Zellzustände auf dem alten Universum ausgeführt werden
  // können
  Universe *next_universe = get_empty_universe(
      universe->height, universe->width, universe->variable_dimension);

  next_universe->frame_count = universe->frame_count + 1;

  // Regeln durchsetzen
  for (int y = 0; y < universe->height; y++) {
    for (int x = 0; x < universe->width; x++) {
      int neighbours = count_neighbours(universe, y, x);

      // Jede lebende Zelle die ...
      if (get_cell_state(universe, y, x) == ALIVE) {
        // ... genau 2 oder 3 Nachbarn hat, überlebt
        if (neighbours == 2 || neighbours == 3) {
          // Zustände werden in dem neuem Universum geändert
          change_cell(next_universe, y, x, ALIVE);
        }
      }
      // Jede tote Zelle die genau 3 Nachbarn hat wird lebendig
      else if (neighbours == 3) {
        change_cell(next_universe, y, x, ALIVE);
      }
    }
  }

  // Speicher wieder freigeben und Resultat in das Ausgangsuniversum kopieren
  free(universe->grid);
  *universe = *next_universe;
  free(next_universe);
}

// Diese Funktion kann die Größe eines Universums anpassen
void resize_universe(Universe **universe_ptr, int new_height, int new_width) {
  if (universe_ptr == NULL || *universe_ptr == NULL)
    exit(EXIT_FAILURE);

  // Absichern gegen zu kleine oder negative Werte
  if(new_height < 3) new_height = 3;
  if(new_width < 3) new_width = 3;

  Universe *old_universe = *universe_ptr;
  Universe *new_universe = get_empty_universe(new_height, new_width,
                                              old_universe->variable_dimension);

  // Bestimmen der Grenzen für das Kopieren (immer das Kleinere)
  int copy_height = min(old_universe->height, new_height);
  int copy_width = min(old_universe->width, new_width);

  // Daten kopieren
  for (int y = 0; y < copy_height; y++) {
    for (int x = 0; x < copy_width; x++) {
      change_cell(new_universe, y, x, get_cell_state(old_universe, y, x));
    }
  }

  // Speicher freigeben
  destroy_universe(old_universe);

  *universe_ptr = new_universe;
}

// Speicher freigeben, wenn ein Universum nicht mehr gebraucht wird
void destroy_universe(Universe *universe) {
  free(universe->grid);
  free(universe);
}
