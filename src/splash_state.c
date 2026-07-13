#include "splash_state.h"

#include <timer.h>

#include "video.h"

static const unsigned int SPLASH_DURATION_SECONDS = 3;

static u64 start_time;
static bool finished;

void splash_state_initialize(void)
{
    start_time = GetTimerSystemTime();
    finished = false;
}

void splash_state_update(void)
{
    u64 elapsed = GetTimerSystemTime() - start_time;

    if (elapsed >= Sec2TimerBusClock(SPLASH_DURATION_SECONDS)) {
        finished = true;
    }
}

void splash_state_render(void)
{
    video_begin_frame();
    video_draw_text(10, 10, "Numbers Station\n\nMilestone 006");
}

void splash_state_shutdown(void)
{
    finished = false;
}

bool splash_state_is_finished(void)
{
    return finished;
}
