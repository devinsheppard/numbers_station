#include "gameplay_state.h"

#include "frame_timer.h"
#include "player.h"
#include "video.h"

enum {
    WORLD_WIDTH = 1600,
    WORLD_HEIGHT = 1200,
    VIEWPORT_WIDTH = 640,
    VIEWPORT_HEIGHT = 448
};

typedef struct WorldRectangle {
    float x;
    float y;
    float width;
    float height;
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} WorldRectangle;

static Player player;
static FrameTimer frame_timer;
static float viewport_x;
static float viewport_y;

static const WorldRectangle landmarks[] = {
    {520.0f, 380.0f, 560.0f, 440.0f, 0x18, 0x38, 0x28},
    {70.0f, 470.0f, 110.0f, 260.0f, 0x88, 0x30, 0x28},
    {1420.0f, 430.0f, 110.0f, 300.0f, 0x28, 0x48, 0x98},
    {680.0f, 60.0f, 240.0f, 90.0f, 0xb0, 0x88, 0x28},
    {670.0f, 1060.0f, 260.0f, 90.0f, 0x70, 0x38, 0x88},
    {20.0f, 20.0f, 120.0f, 120.0f, 0xb0, 0x38, 0x38},
    {1460.0f, 20.0f, 120.0f, 120.0f, 0x38, 0xb0, 0x58},
    {20.0f, 1060.0f, 120.0f, 120.0f, 0x38, 0x68, 0xb0},
    {1460.0f, 1060.0f, 120.0f, 120.0f, 0xb0, 0x98, 0x38}
};

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

static void update_viewport(void)
{
    viewport_x = player.x + player.width * 0.5f - VIEWPORT_WIDTH * 0.5f;
    viewport_y = player.y + player.height * 0.5f - VIEWPORT_HEIGHT * 0.5f;
    viewport_x = clamp(viewport_x, 0.0f, WORLD_WIDTH - VIEWPORT_WIDTH);
    viewport_y = clamp(viewport_y, 0.0f, WORLD_HEIGHT - VIEWPORT_HEIGHT);
}

static void draw_world_rectangle(const WorldRectangle *rectangle)
{
    float left = rectangle->x > viewport_x ? rectangle->x : viewport_x;
    float top = rectangle->y > viewport_y ? rectangle->y : viewport_y;
    float viewport_right = viewport_x + VIEWPORT_WIDTH;
    float viewport_bottom = viewport_y + VIEWPORT_HEIGHT;
    float rectangle_right = rectangle->x + rectangle->width;
    float rectangle_bottom = rectangle->y + rectangle->height;
    float right = rectangle_right < viewport_right ? rectangle_right :
                                                       viewport_right;
    float bottom = rectangle_bottom < viewport_bottom ? rectangle_bottom :
                                                        viewport_bottom;

    if (left >= right || top >= bottom) {
        return;
    }

    video_draw_filled_rect(left - viewport_x, top - viewport_y, right - left,
                           bottom - top, rectangle->red, rectangle->green,
                           rectangle->blue);
}

void gameplay_state_initialize(void)
{
    player_initialize(&player);
    frame_timer_initialize(&frame_timer);
    update_viewport();
}

void gameplay_state_update(void)
{
    frame_timer_update(&frame_timer);
    player_update(&player, frame_timer.delta_seconds);
    update_viewport();
}

void gameplay_state_render(void)
{
    unsigned int index;

    video_begin_frame();
    video_draw_filled_rect(0.0f, 0.0f, VIEWPORT_WIDTH, VIEWPORT_HEIGHT,
                           0x0c, 0x18, 0x20);

    for (index = 0; index < sizeof(landmarks) / sizeof(landmarks[0]); ++index) {
        draw_world_rectangle(&landmarks[index]);
    }

    video_draw_text(2, 1, "Numbers Station\nMilestone 010\nScrolling World");
    video_draw_text(2, 5, "Player world: %d, %d", (int)player.x,
                    (int)player.y);
    video_draw_text(2, 6, "Viewport: %d, %d", (int)viewport_x,
                    (int)viewport_y);
    video_draw_text(2, 8, "D-pad: Move through world");
    player_render(&player, viewport_x, viewport_y);
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
    viewport_x = 0.0f;
    viewport_y = 0.0f;
}
