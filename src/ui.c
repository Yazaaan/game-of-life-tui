#include "../include/ui.h"
#include "../include/config.h"
#include "../include/engine.h"
#include "../include/render.h"
#include "../include/savefile.h"
#include <iso646.h>
#include <ncurses.h>
#include <stdbool.h>
#include <unistd.h>

char global_message[128] = ""; // Global variable for the current info message

// Forward declarations for key actions:
void ui_action_key_clear(Game_State *game);
void ui_action_key_scaling_mode(Game_State *game);
void ui_action_key_save_toggle(Game_State *game);
void ui_action_key_save_to_slot(Game_State *game, int slot);

// Checks whether the terminal is too small for the standard layout, so a corresponding fallback layout can be used.
bool small_terminal_mode(void)
{
    return LINES < TERMINAL_MIN_HEIGHT || COLS < TERMINAL_MIN_WIDTH;
}

// Universe height adjusted to terminal size
int get_available_height(void)
{
    if (small_terminal_mode())
        return LINES - 1;
    return LINES - GRID_START_Y - GRID_MARGIN_Y;
}

// Universe width adjusted to terminal size
int get_available_width(void)
{
    if (small_terminal_mode())
        return COLS;
    return COLS - GRID_START_X - GRID_MARGIN_X;
}

// Initialize a new "game"
void ui_init(Game_State *game)
{
    // Prepare the console
    initscr();
    noecho();
    cbreak();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    curs_set(0);
    start_color();
    use_default_colors();
    init_pair(1, 12, -1); // Set accent color (12) for highlights,
                          // default background (-1)
    init_pair(2, 8, -1);  // Divider lines
    init_pair(3, 9, -1);  // Universe border in fixed mode
    init_pair(4, 10, -1); // Feedback message at bottom
    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
    mouseinterval(0); // Disable double-click delay so the mouse reacts faster

    // Set default values
    game->running = true;
    game->play = false;
    game->simulation_speed = 600;

    int init_height = get_available_height();
    int init_width = get_available_width();
    game->universe =
        get_empty_universe(init_height, init_width, START_VARIABLE_SIZE);

    // Load welcome universe
    load_grid(&game->universe, 10);
    resize_universe(&game->universe, init_height, init_width);
    game->universe->variable_dimension = START_VARIABLE_SIZE;

    set_message("Load universe from slot by pressing any number from 0 to 9.");
    ui_draw(game);
}

// End the ncurses terminal
void ui_cleanup(void)
{
    // Safely exit ncurses mode
    if (!isendwin())
    {
        endwin();
    }
}

// Adjust time per frame
void adjust_simulation_speed(Game_State *game, int adjustment)
{
    int newSpeed = game->simulation_speed + adjustment;
    if (newSpeed <= MAX_SPEED && newSpeed >= MIN_SPEED)
    {
        game->simulation_speed = newSpeed;
    }
    set_message("Changed frame time to %d ms.", game->simulation_speed);
}

// Process keyboard input -> what happens for each key?
void ui_input_process_keyboard(Game_State *game, int input)
{
    switch (input)
    {
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
    case 'n':
        if (!game->play)
            time_step(game->universe);
        set_message("Stepping to next frame");
        break;
    case 'r':
        fill_universe_random(game->universe, RANDOM_CELL_PROBABILITY);
        game->play = false;
        set_message("The Big Bang!");
        break;
    case 'c':
        ui_action_key_clear(game);
        break;
    case 'h':
        ui_action_key_scaling_mode(game);
        break;
    case 's':
        ui_action_key_save_toggle(game);
        break;
    default:
        if (input >= '0' && input <= '9')
        {
            int slot = input - '0';
            ui_action_key_save_to_slot(game, slot);
        }
        break;
    }
}

// Process mouse input
void ui_input_process_mouse(Game_State *game, MEVENT *mouse_event)
{
    // Mouse wheel up
    if (mouse_event->bstate & BUTTON4_PRESSED)
    {
        adjust_simulation_speed(game, -SPEED_INCREMENT);
    }

    // Mouse wheel down
    if (mouse_event->bstate & BUTTON5_PRESSED)
    {
        adjust_simulation_speed(game, SPEED_INCREMENT);
    }

    // Left click
    if (mouse_event->bstate & BUTTON1_PRESSED)
    {
        game->play = false;
        // Store mouse position in the grid
        int click_y;
        int click_x;

        if (small_terminal_mode())
        {
            click_y = mouse_event->y;
            click_x = mouse_event->x;
        }
        else
        {
            click_y = mouse_event->y - GRID_START_Y;
            click_x = mouse_event->x - GRID_START_X;
        }

        if (click_y < game->universe->height && click_y >= 0 &&
            click_x < game->universe->width && click_x >= 0)
        {

            // Toggle state
            bool cell_state = get_cell_state(game->universe, click_y, click_x);
            change_cell(game->universe, click_y, click_x, !cell_state);

            game->universe->frame_count = 0;

            set_message("Edit mode: changed cell at x:%d, y:%d to %s", click_x,
                        click_y, (!cell_state) ? "alive" : "dead");
        }
        else
        {
            set_message("Edit mode: out of bounds at x:%d, y:%d", click_x,
                        click_y);
        }
    }
}

// General input processing
void ui_process_input(Game_State *game)
{
    int input = getch(); // Get the input

    // Process mouse input
    if (input == KEY_MOUSE)
    {
        MEVENT mouse_event;
        if (getmouse(&mouse_event) == OK)
        {
            ui_input_process_mouse(game, &mouse_event);
        }
        // Terminal resize
    }
    else if (input == KEY_RESIZE)
    {
        if (game->universe->variable_dimension)
        {
            resize_universe(&game->universe, get_available_height(),
                            get_available_width());
        }
        // Process keyboard input
    }
    else if (input != ERR)
    {
        ui_input_process_keyboard(game, input);
    }

    ui_draw(game);
}

// Key action for clearing the field
void ui_action_key_clear(Game_State *game)
{
    bool var_dim = game->universe->variable_dimension;
    int new_height, new_width;
    int ccount = game->universe->cells_alive;
    int fcount = game->universe->frame_count;

    // The condition ccount + fcount == 0 triggers an easter egg if cleared twice!
    if (var_dim || ccount + fcount == 0)
    {
        new_height = get_available_height();
        new_width = get_available_width();
    }
    else
    {
        new_height = game->universe->height;
        new_width = game->universe->width;
    }

    if (ccount + fcount == 0)
    {
        load_grid(&game->universe, 11);
        resize_universe(&game->universe, new_height, new_width);
        game->universe->variable_dimension = true;
        set_message("You found the Glider Gun!!!");
    }
    else
    {
        reset_universe(game->universe);
        set_message("Space for something new!");
    }

    game->play = false;
}

// Key action for toggling scaling mode
void ui_action_key_scaling_mode(Game_State *game)
{
    if (game->universe->variable_dimension)
    {
        game->universe->variable_dimension = false;
        set_message("Universe size is now fixed (%d x %d)",
                    game->universe->width, game->universe->height);
    }
    else
    {
        game->universe->variable_dimension = true;
        resize_universe(&game->universe, get_available_height(),
                        get_available_width());

        set_message("Universe size is now depending on terminal size");
    }
}

// Key action for toggling save/load mode
void ui_action_key_save_toggle(Game_State *game)
{
    game->save_mode = !game->save_mode;
    if (game->save_mode)
    {
        set_message(
            "Press any number from 0 to 9 to save current universe to slot.");
    }
    else
    {
        set_message(
            "Load universe from slot by pressing any number from 0 to 9.");
    }
}

// Key action for saving the grid to a slot (0 through 9)
void ui_action_key_save_to_slot(Game_State *game, int slot)
{
    if (game->save_mode)
    {
        if (save_grid(game->universe, slot) == 0)
        {
            set_message("Universe saved to slot %d!", slot);
        }
        else
        {
            set_message(
                "There was an error saving the universe to disk slot %d!",
                slot);
        }
    }
    else
    {
        if (load_grid(&game->universe, slot) == 0)
        {
            set_message("Universe loadeded from slot %d with fixed size!",
                        slot);
        }
        else
        {
            set_message("There was an error loading the universe from slot %d!",
                        slot);
        }
    }
}

// Orchestrates drawing/rendering the user interface
void ui_draw(Game_State *game)
{
    erase();

    // The complete UI is only drawn if the terminal is large enough, otherwise only the most important parts are shown
    if (!small_terminal_mode())
    {
        print_headline();
        print_controls();
        print_stats(game);
        print_dividers();
        print_universe(game->universe, GRID_START_Y, GRID_START_X,
                       GRID_MARGIN_Y, GRID_MARGIN_X);
        print_message(LINES - 1, GRID_START_X, global_message);
    }
    else
    {
        print_universe(game->universe, 0, 0, 1, 0);
        print_message(LINES - 1, 0, global_message);
    }

    refresh();
}
