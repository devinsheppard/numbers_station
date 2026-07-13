#include "input.h"

#include <libpad.h>
#include <loadfile.h>
#include <stddef.h>

enum {
    CONTROLLER_PORT = 0,
    CONTROLLER_SLOT = 0,
    PAD_BUFFER_SIZE = 256
};

static unsigned char pad_buffer[PAD_BUFFER_SIZE] __attribute__((aligned(64)));
static bool port_open;
static InputState state;

typedef struct ButtonName {
    uint16_t mask;
    const char *name;
} ButtonName;

static const ButtonName button_names[] = {
    {PAD_UP, "D-pad Up"},
    {PAD_DOWN, "D-pad Down"},
    {PAD_LEFT, "D-pad Left"},
    {PAD_RIGHT, "D-pad Right"},
    {PAD_CROSS, "Cross"},
    {PAD_CIRCLE, "Circle"},
    {PAD_SQUARE, "Square"},
    {PAD_TRIANGLE, "Triangle"},
    {PAD_START, "Start"},
    {PAD_SELECT, "Select"},
    {PAD_L1, "L1"},
    {PAD_R1, "R1"},
    {PAD_L2, "L2"},
    {PAD_R2, "R2"},
    {PAD_L3, "L3"},
    {PAD_R3, "R3"}
};

static const char *button_name(uint16_t pressed_buttons)
{
    size_t index;

    for (index = 0; index < sizeof(button_names) / sizeof(button_names[0]);
         ++index) {
        if ((pressed_buttons & button_names[index].mask) != 0) {
            return button_names[index].name;
        }
    }

    return state.last_pressed_button;
}

bool input_initialize(void)
{
    state.connected = false;
    state.current_buttons = 0;
    state.pressed_buttons = 0;
    state.released_buttons = 0;
    state.last_pressed_button = "None";
    port_open = false;

    if (SifLoadModule("rom0:SIO2MAN", 0, NULL) < 0) {
        return false;
    }

    if (SifLoadModule("rom0:PADMAN", 0, NULL) < 0) {
        return false;
    }

    if (padInit(0) != 1) {
        return false;
    }

    if (padPortOpen(CONTROLLER_PORT, CONTROLLER_SLOT, pad_buffer) == 0) {
        padEnd();
        return false;
    }

    port_open = true;
    return true;
}

void input_update(void)
{
    struct padButtonStatus buttons;
    uint16_t previous_buttons = state.current_buttons;
    int pad_state = padGetState(CONTROLLER_PORT, CONTROLLER_SLOT);

    state.pressed_buttons = 0;
    state.released_buttons = 0;

    if ((pad_state != PAD_STATE_STABLE && pad_state != PAD_STATE_FINDCTP1) ||
        padRead(CONTROLLER_PORT, CONTROLLER_SLOT, &buttons) == 0) {
        state.connected = false;
        state.current_buttons = 0;
        state.released_buttons = previous_buttons;
        return;
    }

    state.connected = true;
    state.current_buttons = (uint16_t)(0xffffU ^ buttons.btns);
    state.pressed_buttons =
        (uint16_t)(state.current_buttons & (uint16_t)~previous_buttons);
    state.released_buttons =
        (uint16_t)(previous_buttons & (uint16_t)~state.current_buttons);

    if (state.pressed_buttons != 0) {
        state.last_pressed_button = button_name(state.pressed_buttons);
    }
}

const InputState *input_get_state(void)
{
    return &state;
}

void input_shutdown(void)
{
    if (port_open) {
        padPortClose(CONTROLLER_PORT, CONTROLLER_SLOT);
        port_open = false;
    }

    padEnd();
}
