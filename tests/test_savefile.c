
#include "../include/engine.h"
#include "../include/savefile.h"
#include "../lib/unity/unity.h"
#include <unistd.h>

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

// Speichern und anschließendes Laden
void test_save_and_load(void) {
  // Vorbereitung: Ein Muster im Universum erstellen
  change_cell(test_universe, 0, 0, ALIVE);
  change_cell(test_universe, 4, 4, ALIVE);
  int slot = 9;
  // Aufräumen: Test-Dateien nach jedem Test löschen

  // Schritt 1: Speichern
  int save_result = save_grid(test_universe, slot);
  TEST_ASSERT_EQUAL_INT(0, save_result);

  // Schritt 2: Ein zweites, leeres Universum erstellen, um es zu befüllen
  Universe *loading_universe = get_empty_universe(2, 2, true);

  // Schritt 3: Laden
  int load_result = load_grid(&loading_universe, slot);
  TEST_ASSERT_EQUAL_INT(0, load_result);

  // Schritt 4: Überprüfung der Dimensionen und Inhalte
  TEST_ASSERT_EQUAL_INT(5, loading_universe->height);
  TEST_ASSERT_EQUAL_INT(5, loading_universe->width);
  TEST_ASSERT_TRUE(get_cell_state(loading_universe, 0, 0));
  TEST_ASSERT_TRUE(get_cell_state(loading_universe, 4, 4));
  TEST_ASSERT_FALSE(
      get_cell_state(loading_universe, 1, 1)); // Prüfen, ob Rest leer ist

  destroy_universe(loading_universe);
}

// Fehlerfall: Speichern eines NULL-Pointers
void test_save_grid_null(void) {
  int result = save_grid(NULL, 1);
  TEST_ASSERT_EQUAL_INT(1, result);
}

// Fehlerfall: Laden eines ungültigen Slots (> 11)
void test_load_grid_invalid_slot(void) {
  Universe *temp_universe = get_empty_universe(3, 3, true);
  int result = load_grid(&temp_universe, 99); // Slot 99 existiert nicht im Code
  TEST_ASSERT_EQUAL_INT(1, result);
  destroy_universe(temp_universe);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_save_and_load);
  RUN_TEST(test_save_grid_null);
  RUN_TEST(test_load_grid_invalid_slot);

  return UNITY_END();
}
