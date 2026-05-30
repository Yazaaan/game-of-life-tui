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
  mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
  mouseinterval(0);

  settings->running = true;
  settings->play = false;
  settings->simulationSpeed = 600;
  settings->frameCount = 0;
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
  sprintf(game->message, "Frame time: %d ms", game->simulationSpeed);
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
  case 'r':
    fill_universe_random(&state->universe);
    state->play = false;
    state->frameCount = 0;
    sprintf(state->message, "The Big Bang!");
    break;
  case 'c':
    state->universe = get_empty_universe(LINES - GRID_START_Y - GRID_MARGIN_Y,
                                         COLS - GRID_START_X - GRID_MARGIN_X);
    state->play = false;
    state->frameCount = 0;
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

      game->frameCount = 0;

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

void ui_draw(GameState *game) {
  // clear();
  erase();

  attron(A_REVERSE); // Highlight für die Info-Zeile
  mvprintw(1, COLS / 2 - 10, " Conway's GAME OF LIFE ");
  mvprintw(5, 8, "CONTROLS");
  mvprintw(5, COLS - 16, "STATS");
  attroff(A_REVERSE);

  // Stuerungserklärung am linken Rand
  char *keys[] = {"q",
                  "c",
                  "r",
                  "k",
                  "j/mouse wheel down",
                  "l/mouse wheel up",
                  "h",
                  "left mouse button"};
  char *explaination[] = {
      "quit",
      "clear universe",
      "generate random universe",
      "play/pause",
      "slow down simulation",
      "speed up simulation",
      "toggle universe scaling",
      "edit individual cells",
  };
  int line = 7;
  for (int i = 0; i <= 7; i++) {
    attron(A_BOLD);
    mvprintw(line++, 1, "%s:", keys[i]);
    attroff(A_BOLD);
    mvprintw(line++, 2, "%s", explaination[i]);
    line++;
  }

  // Statusinformationen am rechten Rand

  line = 7;
  int print_x = COLS - GRID_MARGIN_X + 1;
  attron(A_BOLD);
  mvprintw(line++, print_x, "%s:", "state");
  attroff(A_BOLD);
  mvprintw(line++, print_x + 1, "%s", game->play ? "running" : "stopped");
  line++;
  attron(A_BOLD);
  mvprintw(line++, print_x, "%s:", "frame duration");
  attroff(A_BOLD);
  mvprintw(line++, print_x + 1, "%d ms", game->simulationSpeed);
  line++;
  attron(A_BOLD);
  mvprintw(line++, print_x, "%s:", "frame");
  attroff(A_BOLD);
  mvprintw(line++, print_x + 1, "%ld", game->frameCount);
  line++;
  attron(A_BOLD);
  mvprintw(line++, print_x, "%s:", "cell count");
  attroff(A_BOLD);
  mvprintw(line++, print_x + 1, "%d", game->universe.cells_alive);
  line++;
  attron(A_BOLD);
  mvprintw(line++, print_x, "%s:", "scaling mode");
  attroff(A_BOLD);
  mvprintw(line++, print_x + 1, "%s", game->variable_dimension ? "dynamic" : "fixed");
  line++;
  attron(A_BOLD);
  mvprintw(line++, print_x, "%s:", "dimensions");
  attroff(A_BOLD);
  mvprintw(line++, print_x + 1, "%d x %d", game->universe.width, game->universe.height);

  // Informationszeile am unteren Rand
  mvprintw(LINES - 1, GRID_START_X, "> %s", game->message);

  // Spielfeld zeichnen
  for (int y = 0; y < game->universe.height; y++) {
    for (int x = 0; x < game->universe.width; x++) {
      if (y + GRID_START_Y < LINES - GRID_MARGIN_Y &&
          x + GRID_START_X < COLS - GRID_MARGIN_X) {
        mvaddch(y + GRID_START_Y, x + GRID_START_X,
                (game->universe.grid[y][x] == ALIVE) ? '#' : ' ');
      }
    }
  }

  refresh();
}
