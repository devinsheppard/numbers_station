#ifndef NUMBERS_STATION_GAMEPLAY_STATE_H
#define NUMBERS_STATION_GAMEPLAY_STATE_H

#include <stdbool.h>

void gameplay_state_initialize(void);
void gameplay_state_update(void);
void gameplay_state_render(void);
void gameplay_state_shutdown(void);
bool gameplay_state_is_main_menu_requested(void);

#endif
