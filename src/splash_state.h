#ifndef NUMBERS_STATION_SPLASH_STATE_H
#define NUMBERS_STATION_SPLASH_STATE_H

#include <stdbool.h>

void splash_state_initialize(void);
void splash_state_update(void);
void splash_state_render(void);
void splash_state_shutdown(void);
bool splash_state_is_finished(void);

#endif
