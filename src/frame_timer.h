#ifndef NUMBERS_STATION_FRAME_TIMER_H
#define NUMBERS_STATION_FRAME_TIMER_H

#include <tamtypes.h>

typedef struct FrameTimer {
    u64 previous_ticks;
    float delta_seconds;
    unsigned int delta_milliseconds;
} FrameTimer;

void frame_timer_initialize(FrameTimer *timer);
void frame_timer_update(FrameTimer *timer);

#endif
