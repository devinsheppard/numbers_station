#ifndef NUMBERS_STATION_VIDEO_H
#define NUMBERS_STATION_VIDEO_H

#include <stdbool.h>
bool video_initialize(void);
void video_begin_frame(void);
void video_draw_text(int x, int y, const char *format, ...)
    __attribute__((format(printf, 3, 4)));
void video_shutdown(void);

#endif
