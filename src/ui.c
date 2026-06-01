#include "../include/ui.h"
#include "../include/engine.h"
#include "../include/timing.h"
#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

void ui_init(GameState *settings) {
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
  init_pair(2, 8, -1);
  init_pair(3, 9, -1);
  mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
  mouseinterval(0);

  settings->running = true;
  settings->play = false;
  settings->simulationSpeed = 600;
  settings->universe = get_empty_universe(LINES - GRID_START_Y - GRID_MARGIN_Y,
                                          COLS - GRID_START_X - GRID_MARGIN_X);
  sprintf(settings->message, "%s", "");
  settings->variable_dimension = true;
  ui_draw(settings);
}

void ui_cleanup() { endwin(); }

void adjust_simulation_speed(GameState *game, int adjustment) {
  int newSpeed = game->simulationSpeed + adjustment;
  if (newSpeed <= MAX_SPEED && newSpeed >= MIN_SPEED) {
    game->simulationSpeed = newSpeed;
  }
  sprintf(game->message, "Changed frame time to %d ms.", game->simulationSpeed);
}

void ui_input_process_keyboard(GameState *state, int input) {
  switch (input) {
  case 'q':
    state->running = false;
    break;
  case 'k':
    state->play = !state->play;
    sprintf(state->message, "%s", (state->play) ? "Simulating" : "Stop");
    break;
  case 'j':
    adjust_simulation_speed(state, SPEED_INCREMENT);
    break;
  case 'l':
    adjust_simulation_speed(state, -SPEED_INCREMENT);
    break;
  case 'm':
    if (!state->play)
      time_step(&state->universe);
    sprintf(state->message, "Stepping a single frame");
    break;
  case 'r':
    fill_universe_random(&state->universe);
    state->play = false;
    sprintf(state->message, "The Big Bang!");
    break;
  case 'c':
    if (state->variable_dimension) {
      state->universe = get_empty_universe(LINES - GRID_START_Y - GRID_MARGIN_Y,
                                           COLS - GRID_START_X - GRID_MARGIN_X);
    } else {
      state->universe =
          get_empty_universe(state->universe.height, state->universe.width);
    }
    state->play = false;
    sprintf(state->message, "%s", "Space for something new!");
    break;
  case 'h':
    if (state->variable_dimension) {
      state->variable_dimension = false;
      sprintf(state->message, "Universe size is now fixed (%d x %d)",
              state->universe.width, state->universe.height);
    } else {
      state->variable_dimension = true;
      resize_universe(&state->universe, LINES - GRID_START_Y - GRID_MARGIN_Y,
                      COLS - GRID_START_X - GRID_MARGIN_X);
      sprintf(state->message,
              "Universe size is now depending on terminal size");
    }
    break;
  }
}

void ui_input_process_mouse(GameState *game, MEVENT *mouse_event) {
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

      game->universe.frameCount = 0;

      snprintf(game->message, 128,
               "Edit mode: changed cell at x:%d, y:%d to %s", click_x, click_y,
               (*cell) ? "alive" : "dead");
    } else {

      snprintf(game->message, 128, "Edit mode: out of bounds at x:%d, y:%d",
               click_x, click_y);
    }
  }
}

void ui_process_input(GameState *game) {
  int input = getch(); // Hole den Input

  // Mauseingabe verarbeiten
  if (input == KEY_MOUSE) {
    MEVENT mouse_event;
    if (getmouse(&mouse_event) == OK) {
      ui_input_process_mouse(game, &mouse_event);
    }
    // Terminal-Resize
  } else if (input == KEY_RESIZE) {
    if (game->variable_dimension) {
      resize_universe(&game->universe, LINES - GRID_START_Y - GRID_MARGIN_Y,
                      COLS - GRID_START_X - GRID_MARGIN_X);
    }
    // Tastatureingabe verarbeiten
  } else if (input != ERR) {
    ui_input_process_keyboard(game, input);
  }

  ui_draw(game);
}

void ui_print_tooltips(void) {
  attron(A_REVERSE);
  mvprintw(4, 9, "CONTROLS");
  attroff(A_REVERSE);

  char *keys[] = {"q",
                  "c",
                  "r",
                  "k",
                  "j/mouse wheel down",
                  "l/mouse wheel up",
                  "m",
                  "h",
                  "left mouse button"};
  char *explaination[] = {
      "quit",
      "clear universe",
      "generate random universe",
      "play/pause",
      "slow down simulation",
      "speed up simulation",
      "move to next frame",
      "toggle universe scaling",
      "edit individual cells",
  };
  int line = 6;
  for (int i = 0; i <= 8; i++) {
    attron(A_BOLD | COLOR_PAIR(1));
    mvprintw(line++, 1, "%s:", keys[i]);
    attroff(A_BOLD | COLOR_PAIR(1));
    mvprintw(line++, 2, "%s", explaination[i]);
    line++;
  }
}

void ui_print_stats(GameState *game) {
  attron(A_REVERSE);
  mvprintw(4, COLS - 12, "STATS");
  attroff(A_REVERSE);

  int line = 6;
  int print_x = COLS - GRID_MARGIN_X + 2;
  attron(A_BOLD | COLOR_PAIR(1));
  mvprintw(line++, print_x, "%s:", "state");
  attroff(A_BOLD | COLOR_PAIR(1));
  mvprintw(line++, print_x + 1, "%s", game->play ? "running" : "stopped");
  line++;
  attron(A_BOLD | COLOR_PAIR(1));
  mvprintw(line++, print_x, "%s:", "frame duration");
  attroff(A_BOLD | COLOR_PAIR(1));
  mvprintw(line++, print_x + 1, "%d ms", game->simulationSpeed);
  line++;
  attron(A_BOLD | COLOR_PAIR(1));
  mvprintw(line++, print_x, "%s:", "frame");
  attroff(A_BOLD | COLOR_PAIR(1));
  mvprintw(line++, print_x + 1, "%ld", game->universe.frameCount);
  line++;
  attron(A_BOLD | COLOR_PAIR(1));
  mvprintw(line++, print_x, "%s:", "cell count");
  attroff(A_BOLD | COLOR_PAIR(1));
  mvprintw(line++, print_x + 1, "%d", game->universe.cells_alive);
  line++;
  attron(A_BOLD | COLOR_PAIR(1));
  mvprintw(line++, print_x, "%s:", "scaling mode");
  attroff(A_BOLD | COLOR_PAIR(1));
  mvprintw(line++, print_x + 1, "%s",
           game->variable_dimension ? "dynamic" : "fixed");
  line++;
  attron(A_BOLD | COLOR_PAIR(1));
  mvprintw(line++, print_x, "%s:", "dimensions");
  attroff(A_BOLD | COLOR_PAIR(1));
  mvprintw(line++, print_x + 1, "%d x %d", game->universe.width,
           game->universe.height);
}

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

void ui_draw(GameState *game) {
  // clear();
  erase();

  attron(A_REVERSE); // Highlight für die Info-Zeile
  mvprintw(1, COLS / 2 - 10, " Conway's GAME OF LIFE ");
  attroff(A_REVERSE);

  // Steuerungserklärung am linken Rand
  ui_print_tooltips();

  // Statusinformationen am rechten Rand
  ui_print_stats(game);

  // Trennlinien zwischen UI-Elementen
  ui_print_dividers();

  // Informationszeile am unteren Rand
  mvprintw(LINES - 1, GRID_START_X, "> %s", game->message);

  // Spielfeld zeichnen
  for (int y = 0; y < game->universe.height; y++) {
    for (int x = 0; x < game->universe.width; x++) {
      if (y + GRID_START_Y < LINES - GRID_MARGIN_Y &&
          x + GRID_START_X < COLS - GRID_MARGIN_X) {
        mvaddch(y + GRID_START_Y, x + GRID_START_X,
                (game->universe.grid[y][x] == ALIVE) ? ACS_BLOCK : ' ');
      }
    }
  }

  // Bei fixer Universumgröße eine Begrenzung zeichnen
  if (!game->variable_dimension) {
    ui_draw_universe_border(game->universe.height, game->universe.width);
  }

  refresh();
}
