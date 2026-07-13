#ifndef NUMBERS_STATION_VIDEO_H
#define NUMBERS_STATION_VIDEO_H

#include <stdbool.h>
#include <stdint.h>

bool video_initialize(void);
void video_render(bool controller_connected, uint64_t frame_count,
                  const char *last_pressed_button);
void video_shutdown(void);

#endif
