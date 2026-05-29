#include "../include/ui.h"
#include "../include/engine.h"
#include "../include/timing.h"
#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>

void ui_init(GameState *settings) {
  initscr();
  noecho();
  cbreak();
  nodelay(stdscr, TRUE);
  keypad(stdscr, TRUE);
  curs_set(0);

  settings->running = true;
  settings->play = false;
  settings->simulationSpeed = 600;
  settings->frameCount = 0;
  sprintf(settings->message, "%s", "");
}

void ui_cleanup() { endwin(); }

void ui_input_process_keyboard(GameState *state) {
  int input = getch();
  int newSpeed = 0;

  switch (input) {
  case 'q':
    state->running = false;
    break;
  case 'k':
    state->play = !state->play;
    sprintf(state->message, "%s", (state->play) ? "Simulating" : "Stop");
    break;
  case 'j':
    newSpeed = state->simulationSpeed + SPEED_INCREMENT;
    if (newSpeed <= MAX_SPEED) {
      state->simulationSpeed = newSpeed;
    }
    sprintf(state->message, "Speed: %d", state->simulationSpeed);
    break;
  case 'l':
    newSpeed = state->simulationSpeed - SPEED_INCREMENT;
    if (newSpeed >= MIN_SPEED) {
      state->simulationSpeed = newSpeed;
    }
    sprintf(state->message, "Speed: %d", state->simulationSpeed);
    break;
  case 'r':
    fill_universe_random(&state->universe);
    state->frameCount = 0;
    break;
  case 'c':
    state->universe = get_empty_universe();
    state->frameCount = 0;
    ui_draw(state);
    break;
  }
}

void ui_draw(GameState *game) {
  // clear();
  erase();

  attron(A_REVERSE); // Highlight für die Info-Zeile
  mvprintw(0, 0,
           "Game Of Life | Press 'q' to quit | 'c' to clear | 'r' to generate "
           "random | 'k' to play/pause | 'j' to slow down | 'l' to speed up");
  attroff(A_REVERSE);
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
