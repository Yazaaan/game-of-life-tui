#include "../include/config.h"
#include "../include/engine.h"
#include <ncurses.h>

int num_controls = 9;
char *controls[][2] = {{"q", "quit"},
                       {"s", "toggle save/load mode"},
                       {"c", "clear universe"},
                       {"r", "generate random universe"},
                       {"k", "play/pause"},
                       {"j/mouse wheel down", "slow down simulation"},
                       {"l/mouse wheel up", "speed up simulation"},
                       {"m", "move to next frame"},
                       {"h", "toggle universe scaling"},
                       {"left mouse button", "edit individual cells"}};

// Drucken der Überschrift
void print_headline() {
  attron(A_BOLD | A_REVERSE);
  mvprintw(1, COLS / 2 - 10, " Conway's GAME OF LIFE ");
  attroff(A_BOLD | A_REVERSE);
}

// Drucken der Eingabetipps am linken Rand
void print_controls() {
  attron(A_REVERSE);
  mvprintw(4, 9, "CONTROLS");
  attroff(A_REVERSE);

  int line = 6;
  for (int i = 0; i <= num_controls; i++) {
    attron(A_BOLD | COLOR_PAIR(1));
    mvprintw(line++, 1, "%s:", controls[i][0]); // Taste
    attroff(A_BOLD | COLOR_PAIR(1));
    mvprintw(line++, 2, "%s", controls[i][1]); // Erklärung
    line++;
  }
}

// Drucken der Statistiken am rechten Rand
void print_stats(Game_State *game) {
  attron(A_REVERSE);
  mvprintw(4, COLS - 12, "STATS");
  attroff(A_REVERSE);

  int line = 6;
  int print_x = COLS - GRID_MARGIN_X + 2;

  attron(A_BOLD | COLOR_PAIR(1));
  mvprintw(line++, print_x, "%s:", "game");
  attroff(A_BOLD | COLOR_PAIR(1));
  mvprintw(line++, print_x + 1, "%s", game->play ? "running" : "stopped");
  line++;
  attron(A_BOLD | COLOR_PAIR(1));
  mvprintw(line++, print_x, "%s:", "frame duration");
  attroff(A_BOLD | COLOR_PAIR(1));
  mvprintw(line++, print_x + 1, "%d ms", game->simulation_speed);
  line++;
  attron(A_BOLD | COLOR_PAIR(1));
  mvprintw(line++, print_x, "%s:", "frame");
  attroff(A_BOLD | COLOR_PAIR(1));
  mvprintw(line++, print_x + 1, "%ld", game->universe->frame_count);
  line++;
  attron(A_BOLD | COLOR_PAIR(1));
  mvprintw(line++, print_x, "%s:", "cell count");
  attroff(A_BOLD | COLOR_PAIR(1));
  mvprintw(line++, print_x + 1, "%d", game->universe->cells_alive);
  line++;
  attron(A_BOLD | COLOR_PAIR(1));
  mvprintw(line++, print_x, "%s:", "scaling mode");
  attroff(A_BOLD | COLOR_PAIR(1));
  mvprintw(line++, print_x + 1, "%s",
           game->universe->variable_dimension ? "dynamic" : "fixed");
  attron(A_BOLD | COLOR_PAIR(1));
  line++;
  mvprintw(line++, print_x, "%s:", "file mode");
  attroff(A_BOLD | COLOR_PAIR(1));
  mvprintw(line++, print_x + 1, (game->save_mode) ? "save" : "load");
  line++;
  attron(A_BOLD | COLOR_PAIR(1));
  mvprintw(line++, print_x, "%s:", "dimensions");
  attroff(A_BOLD | COLOR_PAIR(1));
  mvprintw(line++, print_x + 1, "%d x %d", game->universe->width,
           game->universe->height);
}
