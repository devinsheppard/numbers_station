#include "player.h"

#include <libpad.h>

#include "input.h"
#include "video.h"

enum {
    PLAYER_START_X = 40,
    PLAYER_START_Y = 18,
    PLAYER_MIN_X = 1,
    PLAYER_MAX_X = 78,
    PLAYER_MIN_Y = 10,
    PLAYER_MAX_Y = 27
};

static int clamp(int value, int minimum, int maximum)
{
    if (value < minimum) {
        return minimum;
    }

    if (value > maximum) {
        return maximum;
    }

    return value;
}

void player_initialize(Player *player)
{
    player->x = PLAYER_START_X;
    player->y = PLAYER_START_Y;
    player->movement_speed = 1;
}

void player_update(Player *player)
{
    const InputState *input_state = input_get_state();

    if ((input_state->current_buttons & PAD_UP) != 0) {
        player->y -= player->movement_speed;
    }
    if ((input_state->current_buttons & PAD_DOWN) != 0) {
        player->y += player->movement_speed;
    }
    if ((input_state->current_buttons & PAD_LEFT) != 0) {
        player->x -= player->movement_speed;
    }
    if ((input_state->current_buttons & PAD_RIGHT) != 0) {
        player->x += player->movement_speed;
    }

    player->x = clamp(player->x, PLAYER_MIN_X, PLAYER_MAX_X);
    player->y = clamp(player->y, PLAYER_MIN_Y, PLAYER_MAX_Y);
}

void player_render(const Player *player)
{
    video_draw_text(player->x, player->y, "@");
}
