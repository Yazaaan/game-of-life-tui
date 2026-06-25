#include "../include/engine.h"
#include "../lib/unity/unity.h"

Universe *test_universe = NULL;

// --- Setup and Teardown ---

void setUp(void)
{
    test_universe = get_empty_universe(5, 5, true);
}

void tearDown(void)
{
    if (test_universe != NULL)
    {
        destroy_universe(test_universe);
        test_universe = NULL;
    }
}

// --- Tests ---

// - Change cell state -

// dead to alive
void test_change_cell_dead_to_alive(void)
{
    int target_y = 2;
    int target_x = 2;

    // Set cell to ALIVE
    change_cell(test_universe, target_y, target_x, ALIVE);

    // Cell is ALIVE, cells_alive is 1
    TEST_ASSERT_TRUE(get_cell_state(test_universe, target_y, target_x));
    TEST_ASSERT_EQUAL_INT(1, test_universe->cells_alive);

    target_y = 3;
    target_x = 3;

    // Set another cell to ALIVE
    change_cell(test_universe, target_y, target_x, ALIVE);

    // Cell is ALIVE, cells_alive is 2
    TEST_ASSERT_TRUE(get_cell_state(test_universe, target_y, target_x));
    TEST_ASSERT_EQUAL_INT(2, test_universe->cells_alive);
}

// alive to dead
void test_change_cell_alive_to_dead(void)
{
    int target_y = 1;
    int target_x = 1;

    // Set cell to ALIVE
    change_cell(test_universe, target_y, target_x, ALIVE);
    TEST_ASSERT_EQUAL_INT(1, test_universe->cells_alive);

    // Set cell back to DEAD
    change_cell(test_universe, target_y, target_x, DEAD);

    // Cell is DEAD, cells_alive is back to 0
    TEST_ASSERT_FALSE(get_cell_state(test_universe, target_y, target_x));
    TEST_ASSERT_EQUAL_INT(0, test_universe->cells_alive);
}

// - Count neighbours -

// Cell in the middle without neighbours. Result should be 0.
void test_count_neighbours_empty(void)
{
    int count = count_neighbours(test_universe, 2, 2);
    TEST_ASSERT_EQUAL_INT(0, count);
}

// Exactly one neighbouring cell
void test_count_neighbours_single(void)
{
    change_cell(test_universe, 2, 3, ALIVE);

    int count = count_neighbours(test_universe, 2, 2);
    TEST_ASSERT_EQUAL_INT(1, count);
}

// All neighbouring cells
void test_count_neighbours_full(void)
{
    for (int y = 1; y <= 3; y++)
    {
        for (int x = 1; x <= 3; x++)
        {
            if (y == 2 && x == 2)
                continue; // Do not count the cell itself
            change_cell(test_universe, y, x, ALIVE);
        }
    }

    int count = count_neighbours(test_universe, 2, 2);
    TEST_ASSERT_EQUAL_INT(8, count);
}

// At an edge
void test_count_neighbours_edge(void)
{
    change_cell(test_universe, 0, 2, ALIVE);

    int count = count_neighbours(test_universe, 4, 2);

    TEST_ASSERT_EQUAL_INT(1, count);
}

// At a corner
void test_count_neighbours_corner(void)
{
    change_cell(test_universe, 0, 0, ALIVE);
    change_cell(test_universe, 0, 4, ALIVE);
    change_cell(test_universe, 4, 0, ALIVE);
    change_cell(test_universe, 4, 4, ALIVE);

    int count = count_neighbours(test_universe, 0, 0);
    TEST_ASSERT_EQUAL_INT(3, count);
    count = count_neighbours(test_universe, 0, 4);
    TEST_ASSERT_EQUAL_INT(3, count);
    count = count_neighbours(test_universe, 4, 0);
    TEST_ASSERT_EQUAL_INT(3, count);
    count = count_neighbours(test_universe, 4, 4);
    TEST_ASSERT_EQUAL_INT(3, count);
}

// - Time step -

// Empty universe
void test_time_step_empty(void)
{
    time_step(test_universe);

    TEST_ASSERT_EQUAL_INT(0, test_universe->cells_alive);
    TEST_ASSERT_EQUAL_INT(1, test_universe->frame_count);
}

// Cell dies from overpopulation
void test_time_step_overpopulation(void)
{
    for (int y = 1; y <= 3; y++)
    {
        for (int x = 1; x <= 3; x++)
        {
            change_cell(test_universe, y, x, ALIVE);
        }
    }

    time_step(test_universe);

    // The cell in the middle must be dead
    TEST_ASSERT_FALSE(get_cell_state(test_universe, 2, 2));
}

// Cell is born
void test_time_step_reproduction(void)
{
    change_cell(test_universe, 1, 1, ALIVE);
    change_cell(test_universe, 1, 2, ALIVE);
    change_cell(test_universe, 2, 1, ALIVE);

    time_step(test_universe);

    // A new cell must be born to complete the square
    TEST_ASSERT_TRUE(get_cell_state(test_universe, 2, 2));
}

// A universe with a stable pattern (must not change)
void test_time_step_stable_block(void)
{
    // A block is a 2x2 square of living cells.
    // XX
    // XX

    change_cell(test_universe, 1, 1, ALIVE);
    change_cell(test_universe, 1, 2, ALIVE);
    change_cell(test_universe, 2, 1, ALIVE);
    change_cell(test_universe, 2, 2, ALIVE);

    time_step(test_universe);

    // The cells should still live in the same places
    TEST_ASSERT_TRUE(get_cell_state(test_universe, 1, 1));
    TEST_ASSERT_TRUE(get_cell_state(test_universe, 1, 2));
    TEST_ASSERT_TRUE(get_cell_state(test_universe, 2, 1));
    TEST_ASSERT_TRUE(get_cell_state(test_universe, 2, 2));
}

// Blinker test
void test_time_step_blinker_oscillates(void)
{
    
    // Horizontal line (blinker):
    // (5,4), (5,5), (5,6) are ALIVE
    
    change_cell(test_universe, 2, 1, ALIVE);
    change_cell(test_universe, 2, 2, ALIVE);
    change_cell(test_universe, 2, 3, ALIVE);

    time_step(test_universe);

    // After the step the line should be vertical:
    // (4,5), (5,5), (6,5) are ALIVE

    TEST_ASSERT_TRUE(get_cell_state(test_universe, 1, 2));
    TEST_ASSERT_TRUE(get_cell_state(test_universe, 2, 2));
    TEST_ASSERT_TRUE(get_cell_state(test_universe, 3, 2));
    // The old horizontal endpoints must be dead
    TEST_ASSERT_FALSE(get_cell_state(test_universe, 2, 1));
    TEST_ASSERT_FALSE(get_cell_state(test_universe, 2, 3));

    // In the next step the blinker must be back to its initial state
    time_step(test_universe);
    TEST_ASSERT_TRUE(get_cell_state(test_universe, 2, 1));
    TEST_ASSERT_TRUE(get_cell_state(test_universe, 2, 2));
    TEST_ASSERT_TRUE(get_cell_state(test_universe, 2, 3));
    TEST_ASSERT_FALSE(get_cell_state(test_universe, 1, 2));
    TEST_ASSERT_FALSE(get_cell_state(test_universe, 3, 2));
}

// - Resize -

// Expansion
void test_resize_universe_expansion(void)
{
    // Preparation: Set a cell at edge positions to "ALIVE"
    change_cell(test_universe, 0, 0, ALIVE);
    change_cell(test_universe, 4, 4, ALIVE);

    resize_universe(&test_universe, 10, 10);

    TEST_ASSERT_EQUAL_INT(10, test_universe->width);
    TEST_ASSERT_EQUAL_INT(10, test_universe->height);

    // The old cells must still be present
    TEST_ASSERT_TRUE(get_cell_state(test_universe, 0, 0));
    TEST_ASSERT_TRUE(get_cell_state(test_universe, 4, 4));

    // A new cell in the expanded area must be dead initially
    TEST_ASSERT_FALSE(get_cell_state(test_universe, 9, 9));
}

// Shrinkage
void test_resize_universe_shrinkage(void)
{
    // Preparation: Make a cell at (4,4) alive
    change_cell(test_universe, 4, 4, ALIVE);

    resize_universe(&test_universe, 3, 3);

    TEST_ASSERT_EQUAL_INT(3, test_universe->width);
    TEST_ASSERT_EQUAL_INT(3, test_universe->height);

    // The cell (4,4) is now outside the new grid and must no longer be accessible.
    TEST_ASSERT_FALSE(get_cell_state(test_universe, 2, 2));
}

// Unchanged
void test_resize_universe_unchanged(void)
{
    change_cell(test_universe, 2, 2, ALIVE);
    change_cell(test_universe, 4, 4, ALIVE);

    resize_universe(&test_universe, 5, 5);

    TEST_ASSERT_EQUAL_INT(5, test_universe->height);
    TEST_ASSERT_EQUAL_INT(5, test_universe->width);
    TEST_ASSERT_TRUE(get_cell_state(test_universe, 2, 2));
    TEST_ASSERT_TRUE(get_cell_state(test_universe, 4, 4));
}

// --- Run tests ---
int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_change_cell_dead_to_alive);
    RUN_TEST(test_change_cell_alive_to_dead);

    RUN_TEST(test_count_neighbours_empty);
    RUN_TEST(test_count_neighbours_single);
    RUN_TEST(test_count_neighbours_full);
    RUN_TEST(test_count_neighbours_edge);
    RUN_TEST(test_count_neighbours_corner);

    RUN_TEST(test_time_step_empty);
    RUN_TEST(test_time_step_overpopulation);
    RUN_TEST(test_time_step_reproduction);
    RUN_TEST(test_time_step_stable_block);
    RUN_TEST(test_time_step_blinker_oscillates);

    RUN_TEST(test_resize_universe_expansion);
    RUN_TEST(test_resize_universe_shrinkage);
    RUN_TEST(test_resize_universe_unchanged);

    return UNITY_END();
}
