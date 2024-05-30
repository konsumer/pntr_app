#include <stdio.h>

#define PNTR_APP_IMPLEMENTATION
#define PNTR_ENABLE_DEFAULT_FONT
#define PNTR_ENABLE_VARGS
#define PNTR_DISABLE_MATH
#include "pntr_app.h"

typedef struct AppData {
    pntr_image* logo;
    pntr_font* font;
    bool spacePressed;
    float x;
    pntr_sound* sound;
    pntr_sound* music;
    float velocity;

    pntr_image* droppedImage;
} AppData;

bool Init(pntr_app* app) {
    AppData* appData = pntr_load_memory(sizeof(AppData));
    pntr_app_set_userdata(app, appData);

    appData->logo = pntr_load_image("resources/logo.png");
    if (appData->logo == NULL) {
        pntr_app_log(PNTR_APP_LOG_ERROR, "Failed to load resources/logo.png");
    }
    appData->droppedImage = NULL;
    appData->font = pntr_load_font_default();
    appData->spacePressed = false;
    appData->x = 0;
    appData->velocity = 60.0f;
    appData->sound = pntr_load_sound("resources/sound.wav");
    appData->music = pntr_load_sound("resources/music.ogg");
    pntr_play_sound(appData->music, true);

    pntr_app_set_title(app, "pntr_app: Examples");
    pntr_app_set_icon(app, appData->logo);

    pntr_app_show_mouse(app, false);

    return true;
}

bool Update(pntr_app* app, pntr_image* screen) {
    AppData* appData = (AppData*)pntr_app_userdata(app);
    float deltaTime = pntr_app_delta_time(app);

    // Clear the screen
    pntr_clear_background(screen, PNTR_RAYWHITE);

    // Draw some text
    pntr_draw_text(screen, appData->font, "Congrats! You created your first pntr_app!", 35, screen->height - 30, PNTR_DARKGRAY);

    appData->x += appData->velocity * deltaTime;

    // Draw the logo
    if (appData->logo) {
        pntr_draw_image(screen, appData->logo, (int)appData->x, screen->height / 2 - appData->logo->height / 2);
    }

    if (appData->spacePressed) {
        pntr_draw_text(screen, appData->font, "Space is Pressed!", 10, 10, PNTR_BLACK);
    }
    else {
        pntr_draw_text(screen, appData->font, "Space is not pressed", 10, 10, PNTR_BLACK);
    }

    if (pntr_app_mouse_button_down(app, PNTR_APP_MOUSE_BUTTON_LEFT)) {
        pntr_draw_rectangle_fill(screen, 10, 10, 80, 40, PNTR_RED);
    }

    pntr_draw_circle(screen, pntr_app_mouse_x(app), pntr_app_mouse_y(app), 10, PNTR_BLACK);

    if (pntr_app_mouse_delta_x(app) != 0 || pntr_app_mouse_delta_y(app) != 0)
        pntr_app_log_ex(PNTR_APP_LOG_INFO, "Mouse Delta: (%.2f, %.2f)", pntr_app_mouse_delta_x(app), pntr_app_mouse_delta_y(app));

    if (appData->droppedImage != NULL) {
        pntr_draw_image(screen, appData->droppedImage, 10, 10);
    }

    return true;
}

void Close(pntr_app* app) {
    AppData* appData = (AppData*)pntr_app_userdata(app);

    pntr_unload_image(appData->logo);
    pntr_unload_font(appData->font);
    pntr_unload_sound(appData->sound);
    pntr_unload_sound(appData->music);
    pntr_unload_image(appData->droppedImage);

    pntr_unload_memory(appData);
}

void Event(pntr_app* app, pntr_app_event* event) {
    AppData* appData = (AppData*)pntr_app_userdata(app);
    char message[255];

    switch (event->type) {
        case PNTR_APP_EVENTTYPE_KEY_DOWN: {
            if (event->key == PNTR_APP_KEY_SPACE) {
                appData->spacePressed = true;
            }

            pntr_play_sound(appData->sound, false);

            pntr_app_log_ex(PNTR_APP_LOG_INFO, "Key Pressed: %c", (char)event->key);

            if (event->key == PNTR_APP_KEY_G) {
                pntr_app_set_size(app, 700, 400);
            }

            if (event->key == PNTR_APP_KEY_R) {
                sprintf(message, "Random: %d", pntr_app_random(app, 10, 100));
                pntr_app_log(PNTR_APP_LOG_INFO, message);
            }
        }
        break;

        case PNTR_APP_EVENTTYPE_KEY_UP: {
            if (event->key == PNTR_APP_KEY_SPACE) {
                appData->spacePressed = false;
            }
            pntr_app_log_ex(PNTR_APP_LOG_INFO, "Key Released: %c", (char)event->key);
        }
        break;

        case PNTR_APP_EVENTTYPE_MOUSE_WHEEL: {
            pntr_app_log_ex(PNTR_APP_LOG_INFO, "Wheel: %d", event->mouseWheel);
        }
        break;

        case PNTR_APP_EVENTTYPE_MOUSE_BUTTON_DOWN:
        case PNTR_APP_EVENTTYPE_MOUSE_BUTTON_UP: {
            const char* buttonDown = event->type == PNTR_APP_EVENTTYPE_MOUSE_BUTTON_DOWN ? "Pressed" : "Released";

            const char* button;
            switch (event->mouseButton) {
                case PNTR_APP_MOUSE_BUTTON_LEFT: button = "left"; break;
                case PNTR_APP_MOUSE_BUTTON_RIGHT: button = "right"; break;
                case PNTR_APP_MOUSE_BUTTON_MIDDLE: button = "middle"; break;
                case PNTR_APP_MOUSE_BUTTON_LAST:
                case PNTR_APP_MOUSE_BUTTON_UNKNOWN: button = "unknown"; break;
            }
            pntr_app_log_ex(PNTR_APP_LOG_INFO, "Mouse Button %s: %s", buttonDown, button);

            if (event->type == PNTR_APP_EVENTTYPE_MOUSE_BUTTON_DOWN) {
                if (event->mouseButton == PNTR_APP_MOUSE_BUTTON_LEFT) {
                    pntr_play_sound(appData->sound, false);
                }
                else if (event->mouseButton == PNTR_APP_MOUSE_BUTTON_RIGHT) {
                    pntr_stop_sound(appData->music);
                    pntr_play_sound(appData->music, true);
                }
            }
        }
        break;

        case PNTR_APP_EVENTTYPE_MOUSE_MOVE: {
            //pntr_app_log_ex(PNTR_APP_LOG_INFO, "Mouse Move: (%d, %d) | (%d, %d)", event->mouseX, event->mouseY, event->mouseDeltaX, event->mouseDeltaY);
        }
        break;

        case PNTR_APP_EVENTTYPE_GAMEPAD_BUTTON_UP:
        case PNTR_APP_EVENTTYPE_GAMEPAD_BUTTON_DOWN: {
            pntr_app_log_ex(PNTR_APP_LOG_INFO, "Gamepad: %d. Button: %d %s", event->gamepad, event->gamepadButton, event->type == PNTR_APP_EVENTTYPE_GAMEPAD_BUTTON_DOWN ? "Pressed" : "Released");
        }
        break;

        case PNTR_APP_EVENTTYPE_FILE_DROPPED: {
            sprintf(message, "File Dropped: %s", event->fileDropped);
            pntr_app_log(PNTR_APP_LOG_INFO, message);

            if (appData->droppedImage != NULL) {
                pntr_unload_image(appData->droppedImage);
            }

            appData->droppedImage = pntr_load_image(event->fileDropped);
        }
        break;

        default: {
            pntr_app_log_ex(PNTR_APP_LOG_INFO, "Unknown event: %d", event->type);
        }
        break;
    }
}

pntr_app Main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    return (pntr_app) {
        .width = 400,
        .height = 225,
        .title = "pntr_app: Example",
        .init = Init,
        .update = Update,
        .close = Close,
        .event = Event,
        .fps = 0
    };
}
