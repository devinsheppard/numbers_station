#ifndef NUMBERS_STATION_INPUT_H
#define NUMBERS_STATION_INPUT_H

#include <stdbool.h>
#include <stdint.h>

typedef struct InputState {
    bool connected;
    uint16_t current_buttons;
    uint16_t pressed_buttons;
    uint16_t released_buttons;
    const char *last_pressed_button;
} InputState;

bool input_initialize(void);
void input_update(void);
const InputState *input_get_state(void);
void input_shutdown(void);

#endif
