#include "../include/ui.h"
#include  "../include/prints.h"
#include "../include/config.h"
#include "../include/engine.h"
#include <iso646.h>
#include <ncurses.h>
#include <stdbool.h>
#include <unistd.h>

char global_message[128] = ""; // Globale Varialble für aktuelle Infonachricht

// Initialisierung eines neuen "Spiels" ausführen
void ui_init(Game_State *game) {
  // Konsole bereit machen
  initscr();
  noecho();
  cbreak();
  nodelay(stdscr, TRUE);
  keypad(stdscr, TRUE);
  curs_set(0);
  start_color();
  use_default_colors();
  init_pair(1, 12, -1); // Akzentfarbe (12) setzen (Für Hervorhebungen),
                        // Hintergrund (-1) transparent
  init_pair(2, 8, -1);  // Trennlinien
  init_pair(3, 9, -1);  // Universumsgrenze bei fixem Modus
  init_pair(4, 10, -1); // Feedback-Nachricht unten
  mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
  mouseinterval(0); // Damit die Maus schneller reagiert, wird die Verzögerung
                    // für Doppelklicks ausgeschaltet

  // Standardwerte festlegen
  game->running = true;
  game->play = false;
  game->simulation_speed = 600;
  game->universe =
      get_empty_universe(LINES - GRID_START_Y - GRID_MARGIN_Y,
                         COLS - GRID_START_X - GRID_MARGIN_X, true);

  ui_draw(game);
}

// ncurses-Terminal beenden
void ui_cleanup() { endwin(); }

// Zeit pro Frame anpassen
void adjust_simulation_speed(Game_State *game, int adjustment) {
  int newSpeed = game->simulation_speed + adjustment;
  if (newSpeed <= MAX_SPEED && newSpeed >= MIN_SPEED) {
    game->simulation_speed = newSpeed;
  }
  set_message("Changed frame time to %d ms.", game->simulation_speed);
}

// Tastatureingabe verarbeiten -> Was passiert bei welcher Taste?
void ui_input_process_keyboard(Game_State *game, int input) {
  switch (input) {
  case 'q':
    game->running = false;
    break;
  case 'k':
    game->play = !game->play;
    set_message((game->play) ? "Simulating" : "Stop");
    break;
  case 'j':
    adjust_simulation_speed(game, SPEED_INCREMENT);
    break;
  case 'l':
    adjust_simulation_speed(game, -SPEED_INCREMENT);
    break;
  case 'm':
    if (!game->play)
      time_step(&game->universe);
    set_message("Stepping a single frame");
    break;
  case 'r':
    fill_universe_random(&game->universe, RANDOM_CELL_PROBABILITY);
    game->play = false;
    set_message("The Big Bang!");
    break;
  case 'c':
    if (game->universe.variable_dimension) {
      game->universe = get_empty_universe(LINES - GRID_START_Y - GRID_MARGIN_Y,
                                          COLS - GRID_START_X - GRID_MARGIN_X,
                                          game->universe.variable_dimension);
    } else {
      game->universe =
          get_empty_universe(game->universe.height, game->universe.width,
                             game->universe.variable_dimension);
    }
    game->play = false;
    set_message("Space for something new!");
    break;
  case 'h':
    if (game->universe.variable_dimension) {
      game->universe.variable_dimension = false;
      set_message("Universe size is now fixed (%d x %d)", game->universe.width,
                  game->universe.height);
    } else {
      game->universe.variable_dimension = true;
      resize_universe(&game->universe, LINES - GRID_START_Y - GRID_MARGIN_Y,
                      COLS - GRID_START_X - GRID_MARGIN_X);
      set_message("Universe size is now depending on terminal size");
    }
    break;
  }
}

// Mauseingabe verarbeiten
void ui_input_process_mouse(Game_State *game, MEVENT *mouse_event) {
  // Mausrad nach oben
  if (mouse_event->bstate & BUTTON4_PRESSED) {
    adjust_simulation_speed(game, -SPEED_INCREMENT);
  }

  // Mausrad nach unten
  if (mouse_event->bstate & BUTTON5_PRESSED) {
    adjust_simulation_speed(game, SPEED_INCREMENT);
  }

  // Linksklick
  if (mouse_event->bstate & BUTTON1_PRESSED) {
    game->play = false;
    // Mausposition im Gitter speichern
    int click_y = mouse_event->y - GRID_START_Y;
    int click_x = mouse_event->x - GRID_START_X;

    if (click_y < game->universe.height && click_y >= 0 &&
        click_x < game->universe.width && click_x >= 0) {
      bool *cell = &game->universe.grid[click_y][click_x];

      // Zustand wechseln
      change_cell(&game->universe, click_y, click_x, !*cell);

      game->universe.frame_count = 0;

      set_message("Edit mode: changed cell at x:%d, y:%d to %s", click_x,
                  click_y, (*cell) ? "alive" : "dead");
    } else {
      set_message("Edit mode: out of bounds at x:%d, y:%d", click_x, click_y);
    }
  }
}

// Allgemeine Eingabeverarbeitung
void ui_process_input(Game_State *game) {
  int input = getch(); // Hole den Input

  // Mauseingabe verarbeiten
  if (input == KEY_MOUSE) {
    MEVENT mouse_event;
    if (getmouse(&mouse_event) == OK) {
      ui_input_process_mouse(game, &mouse_event);
    }
    // Terminal-Resize
  } else if (input == KEY_RESIZE) {
    if (game->universe.variable_dimension) {
      resize_universe(&game->universe, LINES - GRID_START_Y - GRID_MARGIN_Y,
                      COLS - GRID_START_X - GRID_MARGIN_X);
    }
    // Tastatureingabe verarbeiten
  } else if (input != ERR) {
    ui_input_process_keyboard(game, input);
  }

  ui_draw(game);
}

// Drucken der Trennlinien zwischen den UI-Elementen
void ui_print_dividers(void) {
  attron(COLOR_PAIR(2));
  // Horizontale Line unter Überschrift
  for (int i = 0; i <= COLS; i++) {
    mvprintw(3, i, "-");
  }
  // Horizontale Line unter Universum
  int line_hight = LINES - GRID_MARGIN_Y;
  for (int i = 0; i <= COLS; i++) {
    mvprintw(line_hight, i, "-");
  }
  // Verticale Linie links
  int line_pos_x = GRID_START_X - 1;
  for (int i = 4; i < line_hight; i++) {
    mvprintw(i, line_pos_x, "|");
  }
  line_pos_x = COLS - GRID_MARGIN_X;
  for (int i = 4; i < line_hight; i++) {
    mvprintw(i, line_pos_x, "|");
  }
  attroff(COLOR_PAIR(2));
}

// Die Universumsgrenze zeichnen
void ui_draw_universe_border(int height, int width) {
  attron(COLOR_PAIR(3));

  // Rechte Wand
  if (COLS > GRID_START_X + width + GRID_MARGIN_X) {
    for (int y = GRID_START_Y;
         y < GRID_START_Y + height && y < LINES - GRID_MARGIN_Y; y++) {
      mvaddch(y, GRID_START_X + width, '.');
    }
  }
  // Untere Wand
  if (LINES > GRID_START_Y + height + GRID_MARGIN_Y) {
    for (int x = GRID_START_X;
         x <= GRID_START_X + width && x < COLS - GRID_MARGIN_X; x++) {
      mvaddch(GRID_START_Y + height, x, '.');
    }
  }
  attroff(COLOR_PAIR(3));
}

// Spielfeld zeichnen
void ui_draw_universe(Universe *universe) {
  for (int y = 0; y < universe->height; y++) {
    for (int x = 0; x < universe->width; x++) {
      if (y + GRID_START_Y < LINES - GRID_MARGIN_Y &&
          x + GRID_START_X < COLS - GRID_MARGIN_X) {
        mvaddch(y + GRID_START_Y, x + GRID_START_X,
                (universe->grid[y][x] == ALIVE) ? ACS_BLOCK : ' ');
      }
    }
  }

  // Bei fixer Universumgröße eine Begrenzung zeichnen
  if (!universe->variable_dimension) {
    ui_draw_universe_border(universe->height, universe->width);
  }
}

// Informationszeile am unteren Rand
void ui_draw_message() {
  attron(COLOR_PAIR(4));
  mvprintw(LINES - 1, GRID_START_X, "> %s", global_message);
  attroff(COLOR_PAIR(4));
}

// Orchestriert das Zeichnen der Benutzerberfläche
void ui_draw(Game_State *game) {
  // clear();
  erase();

  print_headline();
  print_controls();
  print_stats(game);
  ui_print_dividers();
  ui_draw_universe(&game->universe);
  ui_draw_message();

  refresh();
}
