#include "gameplay_state.h"

#include "player.h"
#include "video.h"

static Player player;

void gameplay_state_initialize(void)
{
    player_initialize(&player);
}

void gameplay_state_update(void)
{
    player_update(&player);
}

void gameplay_state_render(void)
{
    video_begin_frame();
    video_draw_text(2, 1,
                    "Numbers Station\nMilestone 006\nGameplay State Active");
    video_draw_text(2, 5, "Player X: %d", player.x);
    video_draw_text(2, 6, "Player Y: %d", player.y);
    video_draw_text(2, 8, "D-pad: Move player");
    player_render(&player);
}

void gameplay_state_shutdown(void)
{
    player.x = 0;
    player.y = 0;
    player.movement_speed = 0;
}
