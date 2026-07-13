#ifndef NUMBERS_STATION_VIDEO_H
#define NUMBERS_STATION_VIDEO_H

#include <stdbool.h>
bool video_initialize(void);
void video_begin_frame(void);
void video_draw_filled_rect(float x, float y, float width, float height,
                            unsigned char red, unsigned char green,
                            unsigned char blue);
void video_draw_test_sprite(float x, float y, float width, float height);
void video_draw_text(int x, int y, const char *format, ...)
    __attribute__((format(printf, 3, 4)));
void video_present_frame(void);
int video_get_display_buffer_index(void);
int video_get_draw_buffer_index(void);
int video_get_test_texture_width(void);
int video_get_test_texture_height(void);
const char *video_get_test_texture_format(void);
void video_shutdown(void);

#endif
