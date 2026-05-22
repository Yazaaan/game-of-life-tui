#include "../include/ui.h"
#include "../include/engine.h"
#include <stdbool.h>
#include <unistd.h>

int main(void){
  bool running = true;
  bool play = false;
  Universe universe;

  while(running){
    int input = ui_get_input();
    switch (input) {
    case 'q':
      running = false;
      break;
    case 'k':
      play = !play;
    }

    if(play){
      time_step(&universe);
    }

    usleep(60000);
  }

  ui_clear();
  return 0;
}
