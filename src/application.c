#include "application.h"

#include <stdbool.h>
#include <sifrpc.h>

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

    application->running = true;
    return true;
}

static void application_update(Application *application)
{
    (void)application;
}

static void application_render(void)
{
    video_render();
}

static void application_shutdown(void)
{
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
        application_render();
    }

    application_shutdown();
    return 0;
}
