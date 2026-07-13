#ifndef NUMBERS_STATION_VIDEO_H
#define NUMBERS_STATION_VIDEO_H

#include <stdbool.h>
bool video_initialize(void);
void video_begin_frame(void);
void video_draw_text(int x, int y, const char *text);
void video_shutdown(void);

#endif
