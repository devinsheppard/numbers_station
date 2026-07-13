#include "frame_timer.h"

#include <timer.h>

enum {
    MAX_DELTA_MILLISECONDS = 100
};

void frame_timer_initialize(FrameTimer *timer)
{
    timer->previous_ticks = GetTimerSystemTime();
    timer->delta_seconds = 0.0f;
    timer->delta_milliseconds = 0;
}

void frame_timer_update(FrameTimer *timer)
{
    const u64 maximum_ticks =
        (u64)(kBUSCLK / 1000) * MAX_DELTA_MILLISECONDS;
    u64 current_ticks = GetTimerSystemTime();
    u64 elapsed_ticks = current_ticks - timer->previous_ticks;

    timer->previous_ticks = current_ticks;

    if (elapsed_ticks > maximum_ticks) {
        elapsed_ticks = maximum_ticks;
    }

    timer->delta_seconds = (float)elapsed_ticks / (float)kBUSCLK;
    timer->delta_milliseconds =
        (unsigned int)(elapsed_ticks / (u64)(kBUSCLK / 1000));
}
