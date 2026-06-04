#include "../include/engine.h"
#include "../include/timing.h"
#include "../include/ui.h"
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
  Game_State
      game; // Erstellen eines neuen Spiels. Hier drinn wird alles passieren!
  long long last_update = 0; // Hilfsvariable für die millis()-Funktion.
                             // Speichert Zeitpunkt der letzten Aktualisierung.

  ui_init(&game);

  atexit(ui_cleanup);

  // Main-Loop
  while (game.running) {
    long long current_time = millis();

    ui_process_input(&game);

    if (game.play) {
      if (current_time - last_update >= game.simulation_speed) {
        last_update = millis();
        time_step(game.universe);
        ui_draw(&game);
      }
    }

    usleep(1000); // Eine Millisekunde warten, damit Schleife nicht konstant
                  // durchrast.
  }

  ui_cleanup();
  destroy_universe(game.universe); // Speicher freigeben
  return EXIT_SUCCESS;
}
