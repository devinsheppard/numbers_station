#include "video.h"

#include <debug.h>
#include <graph.h>

bool video_initialize(void)
{
    init_scr();
    scr_clear();
    return true;
}

void video_begin_frame(void)
{
    graph_wait_vsync();
    scr_clear();
}

void video_draw_text(int x, int y, const char *text)
{
    scr_setXY(x, y);
    scr_printf("%s", text);
}

void video_shutdown(void)
{
    scr_clear();
}
