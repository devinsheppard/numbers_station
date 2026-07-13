#include "application.h"

#include <stdbool.h>
#include <stdint.h>
#include <sifrpc.h>

#include "input.h"
#include "video.h"

typedef struct Application {
    bool running;
    uint64_t frame_count;
} Application;

static bool application_startup(Application *application)
{
    sceSifInitRpc(0);

    if (!video_initialize()) {
        return false;
    }

    if (!input_initialize()) {
        video_shutdown();
        return false;
    }

    application->running = true;
    application->frame_count = 0;
    return true;
}

static void application_update(Application *application)
{
    input_update();
    ++application->frame_count;
}

static void application_render(const Application *application)
{
    const InputState *input_state = input_get_state();

    video_render(input_state->connected, application->frame_count,
                 input_state->last_pressed_button);
}

static void application_shutdown(void)
{
    input_shutdown();
    video_shutdown();
}

int application_run(void)
{
    Application application = {false};

    if (!application_startup(&application)) {
        return 1;
    }

    while (application.running) {
        application_update(&application);
        application_render(&application);
    }

    application_shutdown();
    return 0;
}
