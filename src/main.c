#include "../include/engine.h"
#include "../include/ui.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(void) {
  bool running = true;
  bool play = false;
  char message[30] = "";
  int simulationSpeed = 60000;
  int newSpeed;
  const int simulationSpeedIncrement = 10000;
  const int maxSpeed = 600000;
  const int minSpeed = 10000;

  Universe universe = get_empty_universe();

  ui_init();

  while (running) {
    int input = ui_get_input();
    switch (input) {
    case 'q':
      running = false;
      break;
    case 'k':
      play = !play;
      strcpy(message, (play) ? "Simulating" : "Stop");
      break;
    case 'j':
      newSpeed = simulationSpeed - simulationSpeedIncrement;
      if (newSpeed >= minSpeed) {
        simulationSpeed = newSpeed;
      }
      sprintf(message, "Speed: %d", simulationSpeed);
      break;
    case 'l':
      newSpeed = simulationSpeed + simulationSpeedIncrement;
      if (newSpeed <= maxSpeed) {
        simulationSpeed = newSpeed;
      }
      sprintf(message, "Speed: %d", simulationSpeed);

      break;
    }

    ui_draw(&universe, message);

    if (play) {
      time_step(&universe);
    }

    usleep(simulationSpeed);
  }

  ui_cleanup();
  return 0;
}
