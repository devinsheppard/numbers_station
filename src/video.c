#include "video.h"

#include <debug.h>
#include <graph.h>

bool video_initialize(void)
{
    init_scr();
    scr_clear();
    return true;
}

void video_render(void)
{
    graph_wait_vsync();
    scr_clear();
    scr_setXY(10, 10);
    scr_printf("Numbers Station\n");
    scr_printf("Milestone 003\n");
    scr_printf("Application Framework Initialized\n");
}

void video_shutdown(void)
{
    scr_clear();
}
