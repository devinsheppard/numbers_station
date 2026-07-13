#ifndef NUMBERS_STATION_MAIN_MENU_STATE_H
#define NUMBERS_STATION_MAIN_MENU_STATE_H

#include <stdbool.h>

void main_menu_state_initialize(void);
void main_menu_state_update(void);
void main_menu_state_render(void);
void main_menu_state_shutdown(void);
bool main_menu_state_is_gameplay_requested(void);

#endif
