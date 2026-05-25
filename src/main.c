#include "../include/engine.h"
#include "../include/ui.h"
#include <stdbool.h>
#include <unistd.h>

int main(void) {
  bool running = true;
  bool play = false;
  int simulationSpeed = 60000;

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
      break;
    case 'j':
      simulationSpeed += 10000;
      break;
    case 'l':
      simulationSpeed -= 10000;
      break;
    }

    ui_draw(&universe);

    if (play) {
      time_step(&universe);
    }

    usleep(simulationSpeed);
  }

  ui_cleanup();
  return 0;
}
