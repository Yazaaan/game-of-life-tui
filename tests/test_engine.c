#include "../include/engine.h"
#include "../lib/unity/unity.h"

Universe *test_universe = NULL;

// --- Setup und Teardown ---

void setUp(void) { test_universe = get_empty_universe(5, 5, true); }

void tearDown(void) {
  if (test_universe != NULL) {
    destroy_universe(test_universe);
    test_universe = NULL;
  }
}

// --- Tests ---

// - Zellzustand ändern -

// tot zu lebendig
void test_change_cell_dead_to_alive(void) {
  int target_y = 2;
  int target_x = 2;

  // Zelle auf ALIVE setzen
  change_cell(test_universe, target_y, target_x, ALIVE);

  // Zelle ist ALIVE, cells_alive ist 1
  TEST_ASSERT_TRUE(get_cell_state(test_universe, target_y, target_x));
  TEST_ASSERT_EQUAL_INT(1, test_universe->cells_alive);

  target_y = 3;
  target_x = 3;

  // Weitere Zelle auf ALIVE setzen
  change_cell(test_universe, target_y, target_x, ALIVE);

  // Zelle ist ALIVE, cells_alive ist 2
  TEST_ASSERT_TRUE(get_cell_state(test_universe, target_y, target_x));
  TEST_ASSERT_EQUAL_INT(2, test_universe->cells_alive);
}

// lebendig zu tot
void test_change_cell_alive_to_dead(void) {
  int target_y = 1;
  int target_x = 1;

  // Zelle auf ALIVE setzen
  change_cell(test_universe, target_y, target_x, ALIVE);
  TEST_ASSERT_EQUAL_INT(1, test_universe->cells_alive);

  // Zelle wieder auf DEAD setzen
  change_cell(test_universe, target_y, target_x, DEAD);

  // Zelle ist DEAD, cells_alive ist wieder 0
  TEST_ASSERT_FALSE(get_cell_state(test_universe, target_y, target_x));
  TEST_ASSERT_EQUAL_INT(0, test_universe->cells_alive);
}

// - Nachbarn zählen -

// Zelle in der Mitte ohne Nachbarn. Ergebnis sollte 0 sein.
void test_count_neighbours_empty(void) {
  int count = count_neighbours(test_universe, 2, 2);
  TEST_ASSERT_EQUAL_INT(0, count);
}

// Genau eine Nachbarzelle
void test_count_neighbours_single(void) {
  change_cell(test_universe, 2, 3, ALIVE);

  int count = count_neighbours(test_universe, 2, 2);
  TEST_ASSERT_EQUAL_INT(1, count);
}

// Alle Nachbarzellen
void test_count_neighbours_full(void) {
  for (int y = 1; y <= 3; y++) {
    for (int x = 1; x <= 3; x++) {
      if (y == 2 && x == 2)
        continue; // Die Zelle selbst nicht zählen
      change_cell(test_universe, y, x, ALIVE);
    }
  }

  int count = count_neighbours(test_universe, 2, 2);
  TEST_ASSERT_EQUAL_INT(8, count);
}

// An einer Kante
void test_count_neighbours_edge(void) {
  change_cell(test_universe, 0, 2, ALIVE);

  int count = count_neighbours(test_universe, 4, 2);

  TEST_ASSERT_EQUAL_INT(1, count);
}

// An einer Ecke
void test_count_neighbours_corner(void) {
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

// - Zeitschritt -

// Leeres Universum
void test_time_step_empty(void) {
  time_step(test_universe);

  TEST_ASSERT_EQUAL_INT(0, test_universe->cells_alive);
  TEST_ASSERT_EQUAL_INT(1, test_universe->frame_count);
}

// Zelle stirbt an Überbevölkerung
void test_time_step_overpopulation(void) {
  for (int y = 1; y <= 3; y++) {
    for (int x = 1; x <= 3; x++) {
      change_cell(test_universe, y, x, ALIVE);
    }
  }

  time_step(test_universe);

  // Zelle in der Mitte muss tot sein
  TEST_ASSERT_FALSE(get_cell_state(test_universe, 2, 2));
}

void test_time_step_reproduction(void) {
  change_cell(test_universe, 1, 1, ALIVE);
  change_cell(test_universe, 1, 2, ALIVE);
  change_cell(test_universe, 2, 1, ALIVE);

  time_step(test_universe);

  // Eine neue Zelle muss geboren werden, um das Quadrat zu vervollständigen
  TEST_ASSERT_TRUE(get_cell_state(test_universe, 2, 2));
}

// Ein Universum mit stabilem Muster (darf sich nicht verändern)
void test_time_step_stable_block(void) {
  /*
     Ein Block ist ein 2x2 Quadrat lebender Zellen.
     XX
     XX
  */
  change_cell(test_universe, 1, 1, ALIVE);
  change_cell(test_universe, 1, 2, ALIVE);
  change_cell(test_universe, 2, 1, ALIVE);
  change_cell(test_universe, 2, 2, ALIVE);

  time_step(test_universe);

  // Die Zellen sollten immer noch am gleichen Ort leben
  TEST_ASSERT_TRUE(get_cell_state(test_universe, 1, 1));
  TEST_ASSERT_TRUE(get_cell_state(test_universe, 1, 2));
  TEST_ASSERT_TRUE(get_cell_state(test_universe, 2, 1));
  TEST_ASSERT_TRUE(get_cell_state(test_universe, 2, 2));
}

// Blinker-Test
void test_time_step_blinker_oscillates(void) {
  /*
     Horizontale Linie (Blinker):
     (5,4), (5,5), (5,6) sind ALIVE
  */
  change_cell(test_universe, 2, 1, ALIVE);
  change_cell(test_universe, 2, 2, ALIVE);
  change_cell(test_universe, 2, 3, ALIVE);

  time_step(test_universe);

  /*
     Nach dem Schritt sollte die Linie vertikal sein:
     (4,5), (5,5), (6,5) sind ALIVE
  */
  TEST_ASSERT_TRUE(get_cell_state(test_universe, 1, 2));
  TEST_ASSERT_TRUE(get_cell_state(test_universe, 2, 2));
  TEST_ASSERT_TRUE(get_cell_state(test_universe, 3, 2));
  // Die alten horizontalen Endpunkte müssen tot sein
  TEST_ASSERT_FALSE(get_cell_state(test_universe, 2, 1));
  TEST_ASSERT_FALSE(get_cell_state(test_universe, 2, 3));

  // Im nächsten Schritt muss der Blinker wieder in seinem Ausgangszustand sein
  time_step(test_universe);
  TEST_ASSERT_TRUE(get_cell_state(test_universe, 2, 1));
  TEST_ASSERT_TRUE(get_cell_state(test_universe, 2, 2));
  TEST_ASSERT_TRUE(get_cell_state(test_universe, 2, 3));
  TEST_ASSERT_FALSE(get_cell_state(test_universe, 1, 2));
  TEST_ASSERT_FALSE(get_cell_state(test_universe, 3, 2));
}

// - Resize -

// Vergrößern
void test_resize_universe_expansion(void) {
  // Vorbereitung: Eine Zelle an Randpositionen auf "ALIVE" setzen
  change_cell(test_universe, 0, 0, ALIVE);
  change_cell(test_universe, 4, 4, ALIVE);

  resize_universe(&test_universe, 10, 10);

  TEST_ASSERT_EQUAL_INT(10, test_universe->width);
  TEST_ASSERT_EQUAL_INT(10, test_universe->height);

  // Die alten Zellen müssen noch da sein
  TEST_ASSERT_TRUE(get_cell_state(test_universe, 0, 0));
  TEST_ASSERT_TRUE(get_cell_state(test_universe, 4, 4));

  // Eine neue Zelle im erweiterten Bereich muss am Anfang tot sein
  TEST_ASSERT_FALSE(get_cell_state(test_universe, 9, 9));
}

// Verkleinern
void test_resize_universe_shrinkage(void) {
  // Vorbereitung: Eine Zelle an (4,4) lebendig machen
  change_cell(test_universe, 4, 4, ALIVE);

  resize_universe(&test_universe, 3, 3);

  TEST_ASSERT_EQUAL_INT(3, test_universe->width);
  TEST_ASSERT_EQUAL_INT(3, test_universe->height);

  // Die Zelle (4,4) ist nun außerhalb des neuen Grids und darf nicht mehr
  // zugänglich sein.
  TEST_ASSERT_FALSE(get_cell_state(test_universe, 2, 2));
}

// Gleichbleibend
void test_resize_universe_unchanged(void) {
  change_cell(test_universe, 2, 2, ALIVE);
  change_cell(test_universe, 4, 4, ALIVE);

  resize_universe(&test_universe, 5, 5);

  TEST_ASSERT_EQUAL_INT(5, test_universe->height);
  TEST_ASSERT_EQUAL_INT(5, test_universe->width);
  TEST_ASSERT_TRUE(get_cell_state(test_universe, 2, 2));
  TEST_ASSERT_TRUE(get_cell_state(test_universe, 4, 4));
}

// --- Tests ausführen ---
int main(void) {
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
