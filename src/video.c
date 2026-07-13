#include "video.h"

#include <debug.h>
#include <graph.h>
#include <stdarg.h>

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

void video_draw_text(int x, int y, const char *format, ...)
{
    va_list arguments;

    scr_setXY(x, y);
    va_start(arguments, format);
    scr_vprintf(format, arguments);
    va_end(arguments);
}

void video_shutdown(void)
{
    scr_clear();
}
