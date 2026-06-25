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

// Print the heading
void print_headline(void)
{
    attron(A_BOLD | A_REVERSE);
    mvprintw(1, COLS / 2 - 10, " Conway's GAME OF LIFE ");
    attroff(A_BOLD | A_REVERSE);
}

// Print the tooltips on the left side
void print_controls(void)
{
    attron(A_REVERSE);
    mvprintw(4, 9, "CONTROLS");
    attroff(A_REVERSE);

    int line = 6;
    for (int i = 0; i <= num_controls; i++)
    {
        if (line >= LINES - GRID_MARGIN_Y - 1)
        {
            mvprintw(line - 1, 11, "...");
            break;
        }
        attron(A_BOLD | COLOR_PAIR(1));
        mvprintw(line++, 1, "%s:", controls[i][0]); // key
        attroff(A_BOLD | COLOR_PAIR(1));
        mvprintw(line++, 2, "%s", controls[i][1]); // explanation
        line++;
    }
}

// Print the statistics on the right side
void print_stats(Game_State *game)
{
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

// Print the separator lines between the UI elements
void print_dividers(void)
{
    attron(COLOR_PAIR(2));
    // Horizontal line under the heading
    for (int i = 0; i <= COLS; i++)
    {
        mvprintw(3, i, "-");
    }
    // Horizontal line below the universe
    int line_hight = LINES - GRID_MARGIN_Y;
    for (int i = 0; i <= COLS; i++)
    {
        mvprintw(line_hight, i, "-");
    }
    // Vertical line on the left
    int line_pos_x = GRID_START_X - 1;
    for (int i = 4; i < line_hight; i++)
    {
        mvprintw(i, line_pos_x, "|");
    }
    // Vertical line on the right
    line_pos_x = COLS - GRID_MARGIN_X;
    for (int i = 4; i < line_hight; i++)
    {
        mvprintw(i, line_pos_x, "|");
    }
    attroff(COLOR_PAIR(2));
}

// Draw the boundary of the universe
void print_universe_border(Universe *universe, int offset_y, int offset_x,
                           int margin_y, int margin_x)
{
    attron(COLOR_PAIR(3));

    // Right wall
    if (COLS > offset_x + universe->width + margin_x)
    {
        for (int y = offset_y;
             y < offset_y + universe->height && y < LINES - margin_y; y++)
        {
            mvaddch(y, offset_x + universe->width, '<');
        }
    }
    // Bottom wall
    if (LINES > offset_y + universe->height + margin_y)
    {
        for (int x = offset_x;
             x <= offset_x + universe->width && x < COLS - margin_x; x++)
        {
            mvaddch(offset_y + universe->height, x, '^');
        }
    }
    attroff(COLOR_PAIR(3));
}

// Draw the universe
void print_universe(Universe *universe, int offset_y, int offset_x,
                    int margin_y, int margin_x)
{
    for (int y = 0; y < universe->height; y++)
    {
        for (int x = 0; x < universe->width; x++)
        {
            if (y + offset_y < LINES - margin_y &&
                x + offset_x < COLS - margin_x)
            {
                mvaddch(y + offset_y, x + offset_x,
                        (get_cell_state(universe, y, x) == ALIVE) ? ACS_BLOCK
                                                                  : ' ');
            }
        }
    }

    // Draw a boundary when the universe size is fixed
    if (!universe->variable_dimension)
    {
        print_universe_border(universe, offset_y, offset_x, margin_y, margin_x);
    }
}

// Message bar at the bottom
void print_message(int pos_y, int pos_x, char msg[])
{
    attron(COLOR_PAIR(4));
    mvprintw(pos_y, pos_x, "> %s", msg);
    attroff(COLOR_PAIR(4));
}
