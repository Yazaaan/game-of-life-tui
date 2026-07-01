#include "../include/engine.h"
#include "../include/timing.h"
#include "../include/ui.h"
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    // Creating a new game. This is where it's all going to happen!
    Game_State game;
    long long last_update = 0; // Helper variable for the millis() function.
                               // Stores the time of the last update.

    ui_init(&game);

    atexit(ui_cleanup);

    // Main-Loop
    while (game.running)
    {
        long long current_time = millis();

        ui_process_input(&game);

        if (game.play)
        {
            if (current_time - last_update >= game.simulation_speed)
            {
                last_update = millis();
                time_step(game.universe);
                ui_draw(&game);
            }
        }

        usleep(1000); // Wait one millisecond, so the loop doesn't run
                      // continuously.
    }

    ui_cleanup();
    destroy_universe(game.universe); // Free up memory
    game.universe = NULL;

    return EXIT_SUCCESS;
}
