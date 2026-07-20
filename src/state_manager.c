#include "state_manager.h"

#include "gameplay_state.h"
#include "main_menu_state.h"
#include "splash_state.h"

typedef enum ApplicationState {
    APPLICATION_STATE_NONE,
    APPLICATION_STATE_SPLASH,
    APPLICATION_STATE_MAIN_MENU,
    APPLICATION_STATE_GAMEPLAY
} ApplicationState;

static ApplicationState active_state;

static void state_shutdown(ApplicationState state)
{
    switch (state) {
        case APPLICATION_STATE_SPLASH:
            splash_state_shutdown();
            break;
        case APPLICATION_STATE_MAIN_MENU:
            main_menu_state_shutdown();
            break;
        case APPLICATION_STATE_GAMEPLAY:
            gameplay_state_shutdown();
            break;
        case APPLICATION_STATE_NONE:
            break;
    }
}

static void state_initialize(ApplicationState state)
{
    switch (state) {
        case APPLICATION_STATE_SPLASH:
            splash_state_initialize();
            break;
        case APPLICATION_STATE_MAIN_MENU:
            main_menu_state_initialize();
            break;
        case APPLICATION_STATE_GAMEPLAY:
            gameplay_state_initialize();
            break;
        case APPLICATION_STATE_NONE:
            break;
    }
}

static void state_change(ApplicationState next_state)
{
    state_shutdown(active_state);
    active_state = next_state;
    state_initialize(active_state);
}

void state_manager_initialize(void)
{
    active_state = APPLICATION_STATE_NONE;
    state_change(APPLICATION_STATE_SPLASH);
}

void state_manager_update(void)
{
    switch (active_state) {
        case APPLICATION_STATE_SPLASH:
            splash_state_update();
            if (splash_state_is_finished()) {
                state_change(APPLICATION_STATE_MAIN_MENU);
            }
            break;
        case APPLICATION_STATE_MAIN_MENU:
            main_menu_state_update();
            if (main_menu_state_is_gameplay_requested()) {
                state_change(APPLICATION_STATE_GAMEPLAY);
            }
            break;
        case APPLICATION_STATE_GAMEPLAY:
            gameplay_state_update();
            if (gameplay_state_is_main_menu_requested()) {
                state_change(APPLICATION_STATE_MAIN_MENU);
            }
            break;
        case APPLICATION_STATE_NONE:
            break;
    }
}

void state_manager_render(void)
{
    switch (active_state) {
        case APPLICATION_STATE_SPLASH:
            splash_state_render();
            break;
        case APPLICATION_STATE_MAIN_MENU:
            main_menu_state_render();
            break;
        case APPLICATION_STATE_GAMEPLAY:
            gameplay_state_render();
            break;
        case APPLICATION_STATE_NONE:
            break;
    }
}

void state_manager_shutdown(void)
{
    state_shutdown(active_state);
    active_state = APPLICATION_STATE_NONE;
}
