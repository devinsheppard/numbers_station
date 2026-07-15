#include "player.h"

#include <libpad.h>
#include <math.h>

#include "input.h"
#include "video.h"

static const float WORLD_WIDTH = 1600.0f;
static const float WORLD_HEIGHT = 1200.0f;
static const float PLAYER_SIZE = 24.0f;
static const float PLAYER_SPEED = 180.0f;
static const float ANALOG_DEADZONE = 0.25f;

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
    float digital_x = 0.0f;
    float digital_y = 0.0f;
    float analog_x;
    float analog_y;
    float analog_magnitude;
    float analog_scaled_magnitude = 0.0f;
    float direction_x;
    float direction_y;
    float direction_magnitude;

    if ((input_state->current_buttons & PAD_LEFT) != 0) {
        digital_x -= 1.0f;
    }
    if ((input_state->current_buttons & PAD_RIGHT) != 0) {
        digital_x += 1.0f;
    }
    if ((input_state->current_buttons & PAD_UP) != 0) {
        digital_y -= 1.0f;
    }
    if ((input_state->current_buttons & PAD_DOWN) != 0) {
        digital_y += 1.0f;
    }

    analog_x = input_state->left_stick_x < 0 ?
                   input_state->left_stick_x / 128.0f :
                   input_state->left_stick_x / 127.0f;
    analog_y = input_state->left_stick_y < 0 ?
                   input_state->left_stick_y / 128.0f :
                   input_state->left_stick_y / 127.0f;
    analog_magnitude = sqrtf(analog_x * analog_x + analog_y * analog_y);

    if (analog_magnitude > ANALOG_DEADZONE) {
        analog_scaled_magnitude =
            (analog_magnitude - ANALOG_DEADZONE) / (1.0f - ANALOG_DEADZONE);
        if (analog_scaled_magnitude > 1.0f) {
            analog_scaled_magnitude = 1.0f;
        }
        analog_x = analog_x / analog_magnitude * analog_scaled_magnitude;
        analog_y = analog_y / analog_magnitude * analog_scaled_magnitude;
    } else {
        analog_x = 0.0f;
        analog_y = 0.0f;
    }

    direction_x = digital_x + analog_x;
    direction_y = digital_y + analog_y;
    direction_magnitude =
        sqrtf(direction_x * direction_x + direction_y * direction_y);
    if (direction_magnitude > 1.0f) {
        direction_x /= direction_magnitude;
        direction_y /= direction_magnitude;
    }

    player->x += direction_x * player->movement_speed * delta_seconds;
    player->y += direction_y * player->movement_speed * delta_seconds;

    player->x = clamp(player->x, 0.0f, WORLD_WIDTH - player->width);
    player->y = clamp(player->y, 0.0f, WORLD_HEIGHT - player->height);
}

void player_render(const Player *player, float viewport_x, float viewport_y)
{
    video_draw_test_sprite(player->x - viewport_x, player->y - viewport_y,
                           player->width, player->height);
}
