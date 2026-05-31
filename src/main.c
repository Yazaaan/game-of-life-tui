#include "../include/engine.h"
#include "../include/timing.h"
#include "../include/ui.h"
#include <stdbool.h>
#include <unistd.h>

int main(void) {
  GameState game;
  long long lastUpdate = 0;

  ui_init(&game);

  while (game.running) {
    long long currentTime = millis();
    ui_process_input(&game);

    if (game.play) {
      if (currentTime - lastUpdate >= game.simulationSpeed) {
        lastUpdate = millis();
        game.frameCount++;
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
