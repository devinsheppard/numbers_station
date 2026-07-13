#include "gameplay_state.h"

#include "frame_timer.h"
#include "player.h"
#include "video.h"

static Player player;
static FrameTimer frame_timer;

void gameplay_state_initialize(void)
{
    player_initialize(&player);
    frame_timer_initialize(&frame_timer);
}

void gameplay_state_update(void)
{
    frame_timer_update(&frame_timer);
    player_update(&player, frame_timer.delta_seconds);
}

void gameplay_state_render(void)
{
    video_begin_frame();
    video_draw_text(2, 1,
                    "Numbers Station\nMilestone 008\nDouble Buffering Active");
    video_draw_text(2, 5, "Player X: %d px", (int)player.x);
    video_draw_text(2, 6, "Player Y: %d px", (int)player.y);
    video_draw_text(2, 7, "Delta: %u ms", frame_timer.delta_milliseconds);
    video_draw_text(2, 9, "D-pad: Move rectangle");
    video_draw_text(2, 10, "Display buffer: %d",
                    video_get_display_buffer_index());
    video_draw_text(2, 11, "Draw buffer: %d", video_get_draw_buffer_index());
    player_render(&player);
    video_present_frame();
}

void gameplay_state_shutdown(void)
{
    player.x = 0.0f;
    player.y = 0.0f;
    player.width = 0.0f;
    player.height = 0.0f;
    player.movement_speed = 0.0f;
    frame_timer.previous_ticks = 0;
    frame_timer.delta_seconds = 0.0f;
    frame_timer.delta_milliseconds = 0;
}
