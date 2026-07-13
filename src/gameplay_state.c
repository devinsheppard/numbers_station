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
                    "Numbers Station\nMilestone 009\nTexture Rendering Active");
    video_draw_text(2, 5, "Player X: %d px", (int)player.x);
    video_draw_text(2, 6, "Player Y: %d px", (int)player.y);
    video_draw_text(2, 7, "Display buffer: %d",
                    video_get_display_buffer_index());
    video_draw_text(2, 8, "Draw buffer: %d", video_get_draw_buffer_index());
    video_draw_text(2, 9, "Texture: %dx%d",
                    video_get_test_texture_width(),
                    video_get_test_texture_height());
    video_draw_text(2, 10, "Format: %s", video_get_test_texture_format());
    video_draw_text(2, 12, "D-pad: Move textured sprite");
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
