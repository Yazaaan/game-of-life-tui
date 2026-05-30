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
  settings->universe =
      get_empty_universe(LINES - GRID_START_Y, COLS - GRID_START_X);
  sprintf(settings->message, "%s", "");
  settings->dimensions_variable = true;
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
    sprintf(state->message, "%s", "The Big Bang!");
    break;
  case 'c':
    state->universe =
        get_empty_universe(LINES - GRID_START_Y, COLS - GRID_START_X);
    state->play = false;
    state->frameCount = 0;
    sprintf(state->message, "%s", "Space for something new!");
    break;
  case 'h':
    if (state->dimensions_variable) {
      state->dimensions_variable = false;
    } else {
      state->dimensions_variable = true;
      resize_universe(&state->universe, LINES, COLS);
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
    sprintf(game->message, "%s", "Stoppe zum Bearbeiten");

    // Mausposition im Gitter speichern
    int click_y = mouse_event->y - GRID_START_Y;
    int click_x = mouse_event->x - GRID_START_X;

    if (click_y < game->universe.height && click_y >= 0 &&
        click_x < game->universe.width && click_x >= 0) {
      bool *cell = &game->universe.grid[click_y][click_x];
      *cell = !*cell;

      snprintf(game->message, 128,
               "Editing mode: changed cell at x:%d, y:%d to %s", click_x,
               click_y, (*cell) ? "alive" : "dead");
    } else {

      snprintf(game->message, 128, "Editing mode: out of bounds at x:%d, y:%d",
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
  } else if (game->dimensions_variable && input == KEY_RESIZE) {
    resize_universe(&game->universe, LINES, COLS);
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
  mvprintw(0, 0,
           "Game Of Life | Press 'q' to quit | 'c' to clear | 'r' to generate "
           "random | 'k' to play/pause | 'j' to slow down | 'l' to speed up | "
           "'h' to lock dimensions | "
           "left mouse button to edit | scroll mouse wheel to change speed");
  attroff(A_REVERSE);

  // Simulationsparameter wie FPS darunter drucken

  mvprintw(3, 0, "> %s", game->message);
  // Spielfeld zeichnen
  for (int y = 0; y < game->universe.height; y++) {
    for (int x = 0; x < game->universe.width; x++) {
      mvaddch(y + GRID_START_Y, x + GRID_START_X,
              (game->universe.grid[y][x] == ALIVE) ? '#' : ' ');
    }
  }

  refresh();
}
