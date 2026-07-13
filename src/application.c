#include "application.h"

#include <stdbool.h>
#include <sifrpc.h>

#include "input.h"
#include "state_manager.h"
#include "video.h"

typedef struct Application {
    bool running;
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

    state_manager_initialize();

    application->running = true;
    return true;
}

static void application_update(void)
{
    input_update();
    state_manager_update();
}

static void application_render(void)
{
    state_manager_render();
}

static void application_shutdown(void)
{
    state_manager_shutdown();
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
        application_update();
        application_render();
    }

    application_shutdown();
    return 0;
}
