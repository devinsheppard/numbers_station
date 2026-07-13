#include "main_menu_state.h"

#include <stdbool.h>
#include <libpad.h>

#include "input.h"
#include "video.h"

static bool show_gameplay_placeholder;

void main_menu_state_initialize(void)
{
    show_gameplay_placeholder = false;
}

void main_menu_state_update(void)
{
    const InputState *input_state = input_get_state();

    if ((input_state->pressed_buttons & PAD_START) != 0) {
        show_gameplay_placeholder = true;
    }
}

void main_menu_state_render(void)
{
    video_begin_frame();
    video_draw_text(10, 10, "Numbers Station\n\nMain Menu\n\nPress START");

    if (show_gameplay_placeholder) {
        video_draw_text(10, 20, "Gameplay not yet implemented.");
    }
}

void main_menu_state_shutdown(void)
{
    show_gameplay_placeholder = false;
}
