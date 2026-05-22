#include "../include/ui.h"
#include "../include/engine.h"
#include <stdbool.h>
#include <unistd.h>

int main(void){
  bool running = true;
  bool play = false;
  Universe universe;
  int simulationSpeed = 60000;

  while(running){
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

    if(play){
      time_step(&universe);
    }

    usleep(simulationSpeed);
  }

  ui_clear();
  return 0;
}
