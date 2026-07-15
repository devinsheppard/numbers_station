#include "player.h"

#include <libpad.h>

#include "input.h"
#include "video.h"

static const float WORLD_WIDTH = 1600.0f;
static const float WORLD_HEIGHT = 1200.0f;
static const float PLAYER_SIZE = 24.0f;
static const float PLAYER_SPEED = 180.0f;
static const float DIAGONAL_NORMALIZATION = 0.70710678f;

static float clamp(float value, float minimum, float maximum)
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
    player->width = PLAYER_SIZE;
    player->height = PLAYER_SIZE;
    player->x = (WORLD_WIDTH - player->width) * 0.5f;
    player->y = (WORLD_HEIGHT - player->height) * 0.5f;
    player->movement_speed = PLAYER_SPEED;
}

void player_update(Player *player, float delta_seconds)
{
    const InputState *input_state = input_get_state();
    float direction_x = 0.0f;
    float direction_y = 0.0f;
    float normalization = 1.0f;

    if ((input_state->current_buttons & PAD_LEFT) != 0) {
        direction_x -= 1.0f;
    }
    if ((input_state->current_buttons & PAD_RIGHT) != 0) {
        direction_x += 1.0f;
    }
    if ((input_state->current_buttons & PAD_UP) != 0) {
        direction_y -= 1.0f;
    }
    if ((input_state->current_buttons & PAD_DOWN) != 0) {
        direction_y += 1.0f;
    }

    if (direction_x != 0.0f && direction_y != 0.0f) {
        normalization = DIAGONAL_NORMALIZATION;
    }

    player->x += direction_x * normalization * player->movement_speed *
                 delta_seconds;
    player->y += direction_y * normalization * player->movement_speed *
                 delta_seconds;

    player->x = clamp(player->x, 0.0f, WORLD_WIDTH - player->width);
    player->y = clamp(player->y, 0.0f, WORLD_HEIGHT - player->height);
}

void player_render(const Player *player, float viewport_x, float viewport_y)
{
    video_draw_test_sprite(player->x - viewport_x, player->y - viewport_y,
                           player->width, player->height);
}
