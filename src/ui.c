#include "../include/ui.h"
#include "../include/engine.h"
#include <ncurses.h>
#include <stdbool.h>

void ui_init() {
  initscr();
  noecho();
  cbreak();
  nodelay(stdscr, TRUE);
  keypad(stdscr, TRUE);
  curs_set(0);
}

void ui_cleanup() { endwin(); }

int ui_get_input() {
  int input = getch();
  MEVENT event;

  if (input == KEY_MOUSE) {
    if (getmouse(&event) == OK) {
      int click_x = event.x;
      int click_y = event.y;

      // Klick verarbeiten!!

      return 0;
    }
  }

  return input;
}

void ui_draw(Universe *universe) {
  clear();

  attron(A_REVERSE); // Highlight für die Info-Zeile
  mvprintw(0, 0, "Game Of Life | Press 'q' to quit | 'c' to clear | 'r' to generate random | 'k' to play/pause | 'j' to slow down | 'l' to speed up");
  attroff(A_REVERSE);
  // Spielfeld zeichnen
  for (int y = 0; y < universe->height; y++) {
    for (int x = 0; x < universe->width; x++) {
      mvaddch(y + GRID_START_Y, x + GRID_START_X,
              (universe->grid[y][x] == true) ? '#' : ' ');
    }
  }

  refresh();
}
