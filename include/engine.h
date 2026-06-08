#ifndef ENGINE
#define ENGINE

// Ob eine Zelle lebendig oder tot ist soll durch einen bool repräsentiert
// werden. In C gibt es zwar kein bool, aber hiermit wird die Lesbarkeit des
// Codes verbessert.
#include <stdbool.h>

// Um die Übersicht weiter zu erhöhen weden zwei defines für tot und lebendig
// eingeführt
#define ALIVE true
#define DEAD false

// Dieses Struct enhält die Spielfläche mit den Zellen und wird weil es alles
// Enthält "Universum" genannt
typedef struct {
  bool *grid;
  int width;
  int height;
  bool variable_dimension;
  int cells_alive;
  long frame_count;
} Universe;

// Dieses Struct speichert die Spieleinstellungen und enthält ein eingenes
// Universum
typedef struct {
  Universe *universe;
  bool running;
  bool play;
  int simulation_speed;
  bool save_mode;
} Game_State;

bool get_cell_state(Universe *universe, int y, int x);

Universe *get_empty_universe(int height, int width, bool variable_dimension);

void reset_universe(Universe *old_universe, int target_height,
                         int target_width);

void fill_universe_random(Universe *universe, int ratio);

void change_cell(Universe *universe, int y, int x, bool state);

void time_step(Universe *universe);

Universe *resize_universe(Universe *old_universe, int new_height,
                          int new_width);

void destroy_universe(Universe *universe);

#endif // !ENGINE
