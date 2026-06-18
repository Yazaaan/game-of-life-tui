#ifndef UI
#define UI

#include "engine.h"

// Ersetzt snprintf durch set_message für die Lesbarkeit
#include <stdio.h>

extern char global_message[128];

#define set_message(...)                                                       \
  snprintf(global_message, sizeof(global_message),                             \
           __VA_ARGS__) // Danke Gemini :)

void ui_init(Game_State *game);

void ui_cleanup(void);

void ui_process_input(Game_State *game);

void ui_draw(Game_State *game);

#endif // !UI
