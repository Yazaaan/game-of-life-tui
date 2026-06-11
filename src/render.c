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
                       {"n", "move to next frame"},
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
    if (line >= LINES - GRID_MARGIN_Y - 1) {
      mvprintw(line - 1, 11, "...");
      break;
    }
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

// Drucken der Trennlinien zwischen den UI-Elementen
void print_dividers(void) {
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
void print_universe_border(Universe *universe, int offset_y, int offset_x,
                             int margin_y, int margin_x) {
  attron(COLOR_PAIR(3));

  // Rechte Wand
  if (COLS > offset_x + universe->width + margin_x) {
    for (int y = offset_y;
         y < offset_y + universe->height && y < LINES - margin_y; y++) {
      mvaddch(y, offset_x + universe->width, '<');
    }
  }
  // Untere Wand
  if (LINES > offset_y + universe->height + margin_y) {
    for (int x = offset_x;
         x <= offset_x + universe->width && x < COLS - margin_x; x++) {
      mvaddch(offset_y + universe->height, x, '^');
    }
  }
  attroff(COLOR_PAIR(3));
}

// Spielfeld zeichnen
void print_universe(Universe *universe, int offset_y, int offset_x,
                      int margin_y, int margin_x) {
  for (int y = 0; y < universe->height; y++) {
    for (int x = 0; x < universe->width; x++) {
      if (y + offset_y < LINES - margin_y && x + offset_x < COLS - margin_x) {
        mvaddch(y + offset_y, x + offset_x,
                (get_cell_state(universe, y, x) == ALIVE) ? ACS_BLOCK : ' ');
      }
    }
  }

  // Bei fixer Universumgröße eine Begrenzung zeichnen
  if (!universe->variable_dimension) {
    print_universe_border(universe, offset_y, offset_x, margin_y, margin_x);
  }
}

// Informationszeile am unteren Rand
void print_message(int pos_y, int pos_x, char msg[]) {
  attron(COLOR_PAIR(4));
  mvprintw(pos_y, pos_x, "> %s", msg);
  attroff(COLOR_PAIR(4));
}
