#include "main_menu_state.h"

#include <stdbool.h>
#include <libpad.h>

#include "input.h"
#include "video.h"

static bool gameplay_requested;

void main_menu_state_initialize(void)
{
    gameplay_requested = false;
}

void main_menu_state_update(void)
{
    const InputState *input_state = input_get_state();

    if ((input_state->pressed_buttons & PAD_START) != 0) {
        gameplay_requested = true;
    }
}

void main_menu_state_render(void)
{
    video_begin_frame();
    video_draw_text(10, 10, "Numbers Station\n\nMain Menu\n\nPress START");
    video_present_frame();
}

void main_menu_state_shutdown(void)
{
    gameplay_requested = false;
}

bool main_menu_state_is_gameplay_requested(void)
{
    return gameplay_requested;
}
