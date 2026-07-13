#include "video.h"

#include <debug.h>
#include <graph.h>

bool video_initialize(void)
{
    init_scr();
    scr_clear();
    return true;
}

void video_render(bool controller_connected, uint64_t frame_count,
                  const char *last_pressed_button)
{
    graph_wait_vsync();
    scr_clear();
    scr_setXY(10, 10);
    scr_printf("Numbers Station\n");
    scr_printf("Milestone 004\n");
    scr_printf("Controller Input Initialized\n\n");
    scr_printf("Controller: %s\n",
               controller_connected ? "Connected" : "Not connected");
    scr_printf("Frame: %llu\n", (unsigned long long)frame_count);
    scr_printf("Last pressed: %s\n", last_pressed_button);
}

void video_shutdown(void)
{
    scr_clear();
}
