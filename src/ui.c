#include <ncurses.h>
#include <stdbool.h>
#include "../include/engine.h"

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

void ui_draw(Universe* universe){
  clear();

  // Spielfeld zeichnen
  for(int y = 0; y < universe->height; y++){
    for(int x = 0; x < universe->width; x++){
      mvaddch(y, x, (universe->grid[y][x] == true)? '#' : ' ');
    }
  }

  refresh();
}
