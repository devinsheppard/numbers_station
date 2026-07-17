#include "gameplay_state.h"

#include <libpad.h>
#include <stddef.h>

#include "frame_timer.h"
#include "input.h"
#include "player.h"
#include "video.h"

enum {
    WORLD_WIDTH = 1600,
    WORLD_HEIGHT = 1200,
    VIEWPORT_WIDTH = 640,
    VIEWPORT_HEIGHT = 448,
    ALL_DOCUMENTS_READ = 0x07,
    RADIO_ACTIVATION_RADIUS = 180
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

typedef struct SignalTerminal {
    WorldRectangle rectangle;
    unsigned char activated_red;
    unsigned char activated_green;
    unsigned char activated_blue;
} SignalTerminal;

typedef struct SignalBarrier {
    WorldRectangle rectangle;
    unsigned char disabled_red;
    unsigned char disabled_green;
    unsigned char disabled_blue;
} SignalBarrier;

typedef struct ReadableDocument {
    WorldRectangle rectangle;
    const char *text;
} ReadableDocument;

static Player player;
static FrameTimer frame_timer;
static float viewport_x;
static float viewport_y;
static int player_blocked_x;
static int player_blocked_y;
static int signal_terminal_activated;
static int document_open;
static const char *open_document_text;
static unsigned int documents_read;
static int completion_overlay_open;
static int player_in_extraction_zone;
static float radio_elapsed_seconds;
static unsigned int radio_transmission_index;

static const char objective_activate_terminal[] =
    "Activate the relay terminal.";
static const char objective_search_beyond_barrier[] =
    "Search beyond the barrier.";
static const char objective_investigate_source[] =
    "Investigate the transmission source.";

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

static const WorldRectangle solid_obstacles[] = {
    {650.0f, 450.0f, 36.0f, 300.0f, 0xd0, 0x58, 0x28},
    {900.0f, 520.0f, 300.0f, 36.0f, 0x30, 0xb8, 0xc8},
    {1040.0f, 760.0f, 36.0f, 180.0f, 0xd0, 0x38, 0x88},
    {1040.0f, 904.0f, 220.0f, 36.0f, 0xd0, 0x38, 0x88}
};

static const SignalTerminal signal_terminal = {
    {760.0f, 660.0f, 96.0f, 64.0f, 0x48, 0xd0, 0x70},
    0xf0, 0xd0, 0x40
};

static const SignalBarrier signal_barrier = {
    {900.0f, 620.0f, 36.0f, 220.0f, 0xf0, 0x20, 0x20},
    0x38, 0x48, 0x50
};

static const WorldRectangle extraction_zone = {
    1360.0f, 1020.0f, 80.0f, 80.0f, 0x40, 0xd8, 0xd0
};

static const WorldRectangle radio_source = {
    1510.0f, 1120.0f, 32.0f, 32.0f, 0xd8, 0x48, 0xd0
};

static const char *const radio_transmissions[] = {
    "41729 08314 55190",
    "66302 19447 82016",
    "09531 77208 43665",
    "28144 00973 11852",
    "55017 36490 70221"
};

static const char completion_text[] =
    "MISSION COMPLETE\n"
    "\n"
    "Transmission source located.\n"
    "\n"
    "End of prototype.\n"
    "\n"
    "Press CIRCLE to return.";

static const char relay_field_note_text[] =
    "FIELD NOTE\n"
    "\n"
    "The relay was powered down again\n"
    "at 02:13.\n"
    "\n"
    "Transmission resumed eleven\n"
    "minutes later without input.\n"
    "\n"
    "No external source found.\n"
    "\n"
    "Recommend abandoning the site.\n"
    "\n"
    "E. Mercer\n"
    "\n"
    "Press CIRCLE to close";

static const char maintenance_log_text[] =
    "MAINTENANCE LOG\n"
    "\n"
    "The backup receiver was missing\n"
    "when the morning crew arrived.\n"
    "\n"
    "Mud covered the floor, but no\n"
    "tracks led back to the road.\n"
    "\n"
    "At dusk, its carrier keyed once\n"
    "from somewhere north of the creek.\n"
    "\n"
    "R. Vale\n"
    "\n"
    "Press CIRCLE to close";

static const char frequency_record_text[] =
    "FREQUENCY RECORD\n"
    "\n"
    "The numbers changed tonight.\n"
    "\n"
    "Between the groups, a woman said\n"
    "my name in a voice I remember.\n"
    "\n"
    "The transmitter was unplugged.\n"
    "The speaker kept counting.\n"
    "\n"
    "E. Mercer\n"
    "\n"
    "Press CIRCLE to close";

static const ReadableDocument readable_documents[] = {
    {{980.0f, 680.0f, 48.0f, 48.0f, 0xe0, 0xd4, 0xa8},
     relay_field_note_text},
    {{260.0f, 260.0f, 48.0f, 48.0f, 0xe0, 0xd4, 0xa8},
     maintenance_log_text},
    {{1320.0f, 940.0f, 48.0f, 48.0f, 0xe0, 0xd4, 0xa8},
     frequency_record_text}
};

static const char *current_objective(void)
{
    if (documents_read == ALL_DOCUMENTS_READ) {
        return objective_investigate_source;
    }
    if (signal_terminal_activated) {
        return objective_search_beyond_barrier;
    }
    return objective_activate_terminal;
}

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

static int ranges_overlap(float first_start, float first_end,
                          float second_start, float second_end)
{
    return first_start < second_end && first_end > second_start;
}

static int player_overlaps_signal_terminal(void)
{
    const WorldRectangle *rectangle = &signal_terminal.rectangle;

    return ranges_overlap(player.x, player.x + player.width, rectangle->x,
                          rectangle->x + rectangle->width) &&
           ranges_overlap(player.y, player.y + player.height, rectangle->y,
                          rectangle->y + rectangle->height);
}

static int player_overlaps_document(const ReadableDocument *document)
{
    const WorldRectangle *rectangle = &document->rectangle;

    return ranges_overlap(player.x, player.x + player.width, rectangle->x,
                          rectangle->x + rectangle->width) &&
           ranges_overlap(player.y, player.y + player.height, rectangle->y,
                          rectangle->y + rectangle->height);
}

static int player_overlaps_extraction_zone(void)
{
    return ranges_overlap(player.x, player.x + player.width, extraction_zone.x,
                          extraction_zone.x + extraction_zone.width) &&
           ranges_overlap(player.y, player.y + player.height,
                          extraction_zone.y,
                          extraction_zone.y + extraction_zone.height);
}

static int player_is_near_radio(void)
{
    float player_center_x = player.x + player.width * 0.5f;
    float player_center_y = player.y + player.height * 0.5f;
    float radio_center_x = radio_source.x + radio_source.width * 0.5f;
    float radio_center_y = radio_source.y + radio_source.height * 0.5f;
    float offset_x = player_center_x - radio_center_x;
    float offset_y = player_center_y - radio_center_y;

    return offset_x * offset_x + offset_y * offset_y <=
           RADIO_ACTIVATION_RADIUS * RADIO_ACTIVATION_RADIUS;
}

static void update_radio(void)
{
    if (!player_is_near_radio()) {
        radio_elapsed_seconds = 0.0f;
        radio_transmission_index = 0;
        return;
    }

    radio_elapsed_seconds += frame_timer.delta_seconds;
    if (radio_elapsed_seconds >= 3.0f) {
        radio_elapsed_seconds -= 3.0f;
        ++radio_transmission_index;
        if (radio_transmission_index ==
            sizeof(radio_transmissions) / sizeof(radio_transmissions[0])) {
            radio_transmission_index = 0;
        }
    }
}

static void consider_horizontal_collision(const WorldRectangle *obstacle,
                                          float previous_x, float previous_y,
                                          float proposed_x, float *resolved_x)
{
    float obstacle_right = obstacle->x + obstacle->width;

    if (!ranges_overlap(previous_y, previous_y + player.height, obstacle->y,
                        obstacle->y + obstacle->height)) {
        return;
    }

    if (proposed_x > previous_x &&
        previous_x + player.width <= obstacle->x &&
        proposed_x + player.width > obstacle->x) {
        float boundary = obstacle->x - player.width;

        if (boundary < *resolved_x) {
            *resolved_x = boundary;
            player_blocked_x = 1;
        }
    } else if (proposed_x < previous_x && previous_x >= obstacle_right &&
               proposed_x < obstacle_right) {
        if (obstacle_right > *resolved_x) {
            *resolved_x = obstacle_right;
            player_blocked_x = 1;
        }
    }
}

static float resolve_horizontal(float previous_x, float previous_y,
                                float proposed_x)
{
    float resolved_x = proposed_x;
    unsigned int index;

    player_blocked_x = 0;
    for (index = 0;
         index < sizeof(solid_obstacles) / sizeof(solid_obstacles[0]);
         ++index) {
        consider_horizontal_collision(&solid_obstacles[index], previous_x,
                                      previous_y, proposed_x, &resolved_x);
    }
    if (!signal_terminal_activated) {
        consider_horizontal_collision(&signal_barrier.rectangle, previous_x,
                                      previous_y, proposed_x, &resolved_x);
    }

    return resolved_x;
}

static void consider_vertical_collision(const WorldRectangle *obstacle,
                                        float resolved_x, float previous_y,
                                        float proposed_y, float *resolved_y)
{
    float obstacle_bottom = obstacle->y + obstacle->height;

    if (!ranges_overlap(resolved_x, resolved_x + player.width, obstacle->x,
                        obstacle->x + obstacle->width)) {
        return;
    }

    if (proposed_y > previous_y &&
        previous_y + player.height <= obstacle->y &&
        proposed_y + player.height > obstacle->y) {
        float boundary = obstacle->y - player.height;

        if (boundary < *resolved_y) {
            *resolved_y = boundary;
            player_blocked_y = 1;
        }
    } else if (proposed_y < previous_y && previous_y >= obstacle_bottom &&
               proposed_y < obstacle_bottom) {
        if (obstacle_bottom > *resolved_y) {
            *resolved_y = obstacle_bottom;
            player_blocked_y = 1;
        }
    }
}

static float resolve_vertical(float resolved_x, float previous_y,
                              float proposed_y)
{
    float resolved_y = proposed_y;
    unsigned int index;

    player_blocked_y = 0;
    for (index = 0;
         index < sizeof(solid_obstacles) / sizeof(solid_obstacles[0]);
         ++index) {
        consider_vertical_collision(&solid_obstacles[index], resolved_x,
                                    previous_y, proposed_y, &resolved_y);
    }
    if (!signal_terminal_activated) {
        consider_vertical_collision(&signal_barrier.rectangle, resolved_x,
                                    previous_y, proposed_y, &resolved_y);
    }

    return resolved_y;
}

void gameplay_state_initialize(void)
{
    player_initialize(&player);
    frame_timer_initialize(&frame_timer);
    player_blocked_x = 0;
    player_blocked_y = 0;
    signal_terminal_activated = 0;
    document_open = 0;
    open_document_text = NULL;
    documents_read = 0;
    completion_overlay_open = 0;
    player_in_extraction_zone = 0;
    radio_elapsed_seconds = 0.0f;
    radio_transmission_index = 0;
    update_viewport();
}

void gameplay_state_update(void)
{
    float previous_x = player.x;
    float previous_y = player.y;
    float proposed_x;
    float proposed_y;

    frame_timer_update(&frame_timer);
    if (document_open) {
        if ((input_get_state()->pressed_buttons & PAD_CIRCLE) != 0) {
            document_open = 0;
            open_document_text = NULL;
        }
        return;
    }
    if (completion_overlay_open) {
        if ((input_get_state()->pressed_buttons & PAD_CIRCLE) != 0) {
            completion_overlay_open = 0;
        }
        return;
    }

    player_update(&player, frame_timer.delta_seconds);
    proposed_x = player.x;
    proposed_y = player.y;
    player.x = resolve_horizontal(previous_x, previous_y, proposed_x);
    player.y = resolve_vertical(player.x, previous_y, proposed_y);
    if (!signal_terminal_activated && player_overlaps_signal_terminal() &&
        (input_get_state()->pressed_buttons & PAD_CROSS) != 0) {
        signal_terminal_activated = 1;
    }
    if ((input_get_state()->pressed_buttons & PAD_CROSS) != 0) {
        unsigned int index;

        for (index = 0;
             index < sizeof(readable_documents) / sizeof(readable_documents[0]);
             ++index) {
            if (player_overlaps_document(&readable_documents[index])) {
                document_open = 1;
                open_document_text = readable_documents[index].text;
                documents_read |= 1U << index;
                break;
            }
        }
    }
    update_viewport();
    update_radio();
    {
        int overlaps_extraction = player_overlaps_extraction_zone();

        if (documents_read == ALL_DOCUMENTS_READ && overlaps_extraction &&
            !player_in_extraction_zone) {
            completion_overlay_open = 1;
        }
        player_in_extraction_zone = overlaps_extraction;
    }
}

void gameplay_state_render(void)
{
    WorldRectangle terminal_rectangle = signal_terminal.rectangle;
    WorldRectangle barrier_rectangle = signal_barrier.rectangle;
    unsigned int index;

    if (completion_overlay_open) {
        video_begin_frame();
        video_draw_filled_rect(0.0f, 0.0f, VIEWPORT_WIDTH, VIEWPORT_HEIGHT,
                               0x08, 0x0c, 0x10);
        video_draw_text(4, 3, "%s", completion_text);
        video_present_frame();
        return;
    }

    if (document_open) {
        video_begin_frame();
        video_draw_filled_rect(0.0f, 0.0f, VIEWPORT_WIDTH, VIEWPORT_HEIGHT,
                               0x08, 0x0c, 0x10);
        video_draw_text(4, 3, "%s", open_document_text);
        video_present_frame();
        return;
    }

    video_begin_frame();
    video_draw_filled_rect(0.0f, 0.0f, VIEWPORT_WIDTH, VIEWPORT_HEIGHT,
                           0x0c, 0x18, 0x20);

    for (index = 0; index < sizeof(landmarks) / sizeof(landmarks[0]); ++index) {
        draw_world_rectangle(&landmarks[index]);
    }

    if (signal_terminal_activated) {
        terminal_rectangle.red = signal_terminal.activated_red;
        terminal_rectangle.green = signal_terminal.activated_green;
        terminal_rectangle.blue = signal_terminal.activated_blue;
    }
    draw_world_rectangle(&terminal_rectangle);

    if (signal_terminal_activated) {
        barrier_rectangle.red = signal_barrier.disabled_red;
        barrier_rectangle.green = signal_barrier.disabled_green;
        barrier_rectangle.blue = signal_barrier.disabled_blue;
    }
    draw_world_rectangle(&barrier_rectangle);

    for (index = 0;
         index < sizeof(readable_documents) / sizeof(readable_documents[0]);
         ++index) {
        draw_world_rectangle(&readable_documents[index].rectangle);
    }

    draw_world_rectangle(&extraction_zone);
    draw_world_rectangle(&radio_source);

    for (index = 0;
         index < sizeof(solid_obstacles) / sizeof(solid_obstacles[0]);
         ++index) {
        draw_world_rectangle(&solid_obstacles[index]);
    }

    video_draw_text(2, 1,
                    "Numbers Station\nMilestone 019\nAmbient Radio");
    video_draw_text(2, 5, "Player world: %d, %d", (int)player.x,
                    (int)player.y);
    video_draw_text(2, 6, "Viewport: %d, %d", (int)viewport_x,
                    (int)viewport_y);
    video_draw_text(2, 7, "Blocked X/Y: %s / %s",
                    player_blocked_x ? "yes" : "no",
                    player_blocked_y ? "yes" : "no");
    video_draw_text(2, 9, "D-pad / left stick: Move and slide");
    video_draw_text(2, 10, "Barrier: %s",
                    signal_terminal_activated ? "DISABLED" : "ACTIVE");
    if (signal_terminal_activated) {
        video_draw_text(2, 11, "Signal terminal activated.");
    } else if (player_overlaps_signal_terminal()) {
        video_draw_text(2, 11, "Press CROSS to activate.");
    }
    for (index = 0;
         index < sizeof(readable_documents) / sizeof(readable_documents[0]);
         ++index) {
        if (player_overlaps_document(&readable_documents[index])) {
            video_draw_text(2, 12, "Press CROSS to read.");
            break;
        }
    }
    video_draw_text(2, 14, "Objective: %s", current_objective());
    if (player_is_near_radio()) {
        video_draw_text(28, 1, "RADIO: %s",
                        radio_transmissions[radio_transmission_index]);
    }
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
    player_blocked_x = 0;
    player_blocked_y = 0;
    documents_read = 0;
    completion_overlay_open = 0;
    player_in_extraction_zone = 0;
    radio_elapsed_seconds = 0.0f;
    radio_transmission_index = 0;
}
