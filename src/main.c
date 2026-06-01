#include "../include/engine.h"
#include "../include/timing.h"
#include "../include/ui.h"
#include <stdbool.h>
#include <unistd.h>

int main(void) {
  Game_State game;
  long long last_update = 0;

  ui_init(&game);

  while (game.running) {
    long long current_time = millis();
    ui_process_input(&game);

    if (game.play) {
      if (current_time - last_update >= game.simulation_speed) {
        last_update = millis();
        time_step(&game.universe);
        ui_draw(&game);
      }
    }

    usleep(1000); // Eine Millisekunde warten, damit Schleife nicht konstant
                  // durchrast.
  }

  ui_cleanup();
  destroy_universe(&game.universe);
  return 0;
}
