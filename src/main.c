#include "../include/engine.h"
#include "../include/timing.h"
#include "../include/ui.h"
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

int main(void) {
  bool running = true;
  bool play = false;
  int simulationSpeed = 600;
  int newSpeed;
  const int simulationSpeedIncrement = 100;
  const int maxSpeed = 6000;
  const int minSpeed = 100;

  long long lastUpdate = 0;

  int testCount = 0;

  Universe universe = get_empty_universe();

  ui_init();
  char message[30] = "";
  ui_set_msg(message);

  while (running) {
    long long currentTime = millis();
    int input = ui_get_input();

    switch (input) {
    case 'q':
      running = false;
      break;
    case 'k':
      play = !play;
      sprintf(message, "%s", (play) ? "Simulating" : "Stop");
      break;
    case 'j':
      newSpeed = simulationSpeed + simulationSpeedIncrement;
      if (newSpeed <= maxSpeed) {
        simulationSpeed = newSpeed;
      }
      sprintf(message, "Speed: %d", simulationSpeed);
      break;
    case 'l':
      newSpeed = simulationSpeed - simulationSpeedIncrement;
      if (newSpeed >= minSpeed) {
        simulationSpeed = newSpeed;
      }
      sprintf(message, "Speed: %d", simulationSpeed);

      break;
    }

    if (play) {
      if (currentTime - lastUpdate >= simulationSpeed) {
        sprintf(message, "%d", testCount++);
        time_step(&universe);
      }
    }

    ui_draw(&universe, message);

    usleep(1000); // Eine Millisekunde warten, damit Schleife nicht konstant
                  // durchrast.
  }

  ui_cleanup();
  return 0;
}
