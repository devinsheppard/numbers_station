#include "main_menu_state.h"

#include <stdbool.h>
#include <libpad.h>

#include "input.h"
#include "video.h"

static bool gameplay_requested;
static bool controls_open;

void main_menu_state_initialize(void)
{
    gameplay_requested = false;
    controls_open = false;
}

void main_menu_state_update(void)
{
    const InputState *input_state = input_get_state();

    if (controls_open) {
        if ((input_state->pressed_buttons & PAD_CIRCLE) != 0) {
            controls_open = false;
        }
        return;
    }

    if ((input_state->pressed_buttons & PAD_TRIANGLE) != 0) {
        controls_open = true;
    } else if ((input_state->pressed_buttons & PAD_START) != 0) {
        gameplay_requested = true;
    }
}

void main_menu_state_render(void)
{
    video_begin_frame();
    if (controls_open) {
        video_draw_text(8, 7,
                        "CONTROLS\n\n"
                        "D-PAD / LEFT STICK   Move\n"
                        "CROSS                Interact\n"
                        "CIRCLE               Close overlays\n"
                        "START                Pause / Resume\n\n"
                        "Press CIRCLE to return");
    } else {
        video_draw_text(10, 9,
                        "Numbers Station\n\nMain Menu\n\n"
                        "Press START to begin\n"
                        "Press TRIANGLE for controls");
    }
    video_present_frame();
}

void main_menu_state_shutdown(void)
{
    gameplay_requested = false;
    controls_open = false;
}

bool main_menu_state_is_gameplay_requested(void)
{
    return gameplay_requested;
}
