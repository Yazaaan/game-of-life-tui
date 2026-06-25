#include "../include/engine.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define min(a, b) ((a) < (b) ? (a) : (b))

inline bool get_cell_state(const Universe *universe, int y, int x)
{
    if ((unsigned int) y >= universe->height ||
        (unsigned int) x >= universe->width)
        return DEAD;
    return universe->grid[y * universe->width + x];
}

// Changes the state of a cell in a universe
void change_cell(Universe *universe, int y, int x, bool new_state)
{
    if (y >= universe->height || x >= universe->width || y < 0 || x < 0)
        exit(EXIT_FAILURE);

    bool state = get_cell_state(universe, y, x);
    universe->grid[y * universe->width + x] = new_state;

    // Nur wenn der neue Zustand vom alten Zustand unterschiedlich ist und die
    // Zelle lebendig ist, soll gezählt werden
    if (state != new_state)
    {
        if (new_state)
        {
            universe->cells_alive++;
        }
        else
        {
            universe->cells_alive--;
        }
    }
}

// Create an empty universe in which every cell is dead
Universe *get_empty_universe(int height, int width, bool variable_dimension)
{
    Universe *universe = malloc(sizeof(Universe));
    if (universe == NULL)
    {
        exit(EXIT_FAILURE);
    }

    universe->width = width;
    universe->height = height;
    universe->variable_dimension = variable_dimension;
    universe->cells_alive = 0;
    universe->frame_count = 0;

    // Reserve memory for the new cell array (height * width * memory size)
    universe->grid = (bool *) calloc(height * width, sizeof(bool));
    if (universe->grid == NULL)
    {
        free(universe);
        exit(EXIT_FAILURE);
    }

    return universe;
}

// Custom function to clear a universe
void reset_universe(Universe *universe)
{
    if (universe == NULL)
        exit(EXIT_FAILURE);

    memset(universe->grid, DEAD,
           universe->height * universe->width *
               sizeof(bool)); // Fills everything from the address of the grid
                              // to the end of the grid with zeros.

    universe->cells_alive = 0;
    universe->frame_count = 0;
}

// Fill the universe with random cell states
void fill_universe_random(Universe *universe, int probability_percent)
{
    if (universe == NULL)
        exit(EXIT_FAILURE);

    // Set the seed for the random number generator to the system time plus the
    // number of cells from the old universe. Prevents the cells from being
    // arranged in the same order when the operation is performed multiple times
    // in one second.
    srand(time(NULL) + universe->cells_alive);

    universe->frame_count = 0;

    for (int y = 0; y < universe->height; y++)
    {
        for (int x = 0; x < universe->width; x++)
        {
            int result = (rand() % 100 < probability_percent) ? ALIVE : DEAD;
            change_cell(universe, y, x, result);
        }
    }
}

// Counts the neighbors around a cell and returns the result
int count_neighbours(const Universe *universe, int y, int x)
{
    if (universe == NULL)
        return 0;

    int count = 0;

    for (int relative_y = -1; relative_y <= 1; relative_y++)
    {
        for (int relative_x = -1; relative_x <= 1; relative_x++)
        {
            if (relative_x == 0 && relative_y == 0)
                continue; // Should not count itself
            int check_y = y + relative_y;
            int check_x = x + relative_x;

            // Be aware of the boundaries of the universe; if you cross them,
            // look to the other side
            if (check_y == universe->height)
            {
                check_y = 0;
            }
            else if (check_y == -1)
            {
                check_y = universe->height - 1;
            }

            if (check_x == universe->width)
            {
                check_x = 0;
            }
            else if (check_x == -1)
            {
                check_x = universe->width - 1;
            }

            // If a cell is found at the test position, increment the counter
            if (get_cell_state(universe, check_y, check_x) == true)
                count++;
        }
    }

    return count;
}

// Here, the simulation advances by one time step into the future, and the new
// state for each cell is determined based on the game's rules
void time_step(Universe *universe)
{
    if (universe == NULL)
        exit(EXIT_FAILURE);

    // A new universe is created for the new frame so that the new cell states
    // can be calculated using the old universe
    Universe *next_universe = get_empty_universe(
        universe->height, universe->width, universe->variable_dimension);

    next_universe->frame_count = universe->frame_count + 1;

    // Enforce rules
    for (int y = 0; y < universe->height; y++)
    {
        for (int x = 0; x < universe->width; x++)
        {
            int neighbours = count_neighbours(universe, y, x);

            // Every living cell that ...
            if (get_cell_state(universe, y, x) == ALIVE)
            {
                // ... has exactly 2 or 3 neighbors, survives
                if (neighbours == 2 || neighbours == 3)
                {
                    // States are altered in the new universe
                    change_cell(next_universe, y, x, ALIVE);
                }
            }
            // Any dead cell that has exactly 3 neighbors becomes alive
            else if (neighbours == 3)
            {
                change_cell(next_universe, y, x, ALIVE);
            }
        }
    }

    // Release the memory and copy the result to the source universe
    free(universe->grid);
    *universe = *next_universe;
    free(next_universe);
}

// This function can adjust the size of a universe
void resize_universe(Universe **universe_ptr, int new_height, int new_width)
{
    if (universe_ptr == NULL || *universe_ptr == NULL)
        exit(EXIT_FAILURE);

    // Prevent values that are too small or negative
    if (new_height < 3)
        new_height = 3;
    if (new_width < 3)
        new_width = 3;

    Universe *old_universe = *universe_ptr;
    Universe *new_universe = get_empty_universe(
        new_height, new_width, old_universe->variable_dimension);

    // Set the limits for copying (always the smaller value)
    int copy_height = min(old_universe->height, new_height);
    int copy_width = min(old_universe->width, new_width);

    // Copy data
    for (int y = 0; y < copy_height; y++)
    {
        for (int x = 0; x < copy_width; x++)
        {
            change_cell(new_universe, y, x, get_cell_state(old_universe, y, x));
        }
    }

    // Free memory
    destroy_universe(old_universe);

    *universe_ptr = new_universe;
}

// Free up memory when a universe is no longer needed
void destroy_universe(Universe *universe)
{
    free(universe->grid);
    free(universe);
}
