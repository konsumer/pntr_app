#ifdef PNTR_APP_RGFW
#ifndef PNTR_APP_RGFW_H__
#define PNTR_APP_RGFW_H__

#ifndef RGFW_HEADER
#include "RGFW.h"
#endif

typedef struct pntr_app_rgfw_platform {
    int mouseX;
    int mouseY;
    bool keysEnabled[PNTR_APP_KEY_LAST];
    bool mouseButtonsPressed[PNTR_APP_MOUSE_BUTTON_LAST];
    RGFW_window* window;
} pntr_app_rgfw_platform;

#endif  // PNTR_APP_RGFW_H__

#if defined(PNTR_APP_IMPLEMENTATION) && !defined(PNTR_APP_HEADER_ONLY)
#ifndef PNTR_APP_RGFW_IMPLEMENTATION_ONCE
#define PNTR_APP_RGFW_IMPLEMENTATION_ONCE

bool pntr_app_platform_events(pntr_app* app) {
	if (app == NULL || app->platform == NULL) {
  	return false;
  }

  pntr_app_rgfw_platform* platform = (pntr_app_rgfw_platform*)app->platform;
  while(RGFW_window_checkEvent(platform->window)) {
		if (platform->window->event.type == RGFW_quit) {
			return false;
		}
	}
  
  // TODO: process other events

  return true;
}

/**
 * Pushes the given image to the screen.
 */
bool pntr_app_platform_render(pntr_app* app) {
	if (app == NULL || app->screen == NULL) {
  	return false;
  }

  // TODO: do render

  return true;
}


bool pntr_app_platform_init(pntr_app* app) {
	if (app == NULL) {
  	return false;
	}
	app->platform = pntr_load_memory(sizeof(pntr_app_rgfw_platform));
  if (app->platform == NULL) {
      return false;
  }
  pntr_app_rgfw_platform* platform = (pntr_app_rgfw_platform*)app->platform;
  platform->window = RGFW_createWindow(app->title, RGFW_RECT(app->width, app->height,app->width, app->height), (uint64_t)RGFW_CENTER);
	
	
	return true;
}


void pntr_app_platform_close(pntr_app* app) {
	// TODO: shutdown

  if (app == NULL) {
    return;
  }

  pntr_app_rgfw_platform* platform = (pntr_app_rgfw_platform*)app->platform;
  RGFW_window_close(platform->window);

  pntr_unload_memory(app->platform);
  app->platform = NULL;
}

bool pntr_app_platform_update_delta_time(pntr_app* app) {
  // TODO: Make CLI delta time get the actual delta time.
  if (app->fps <= 0) {
    app->deltaTime = 0.1f;
    return true;
	}

  app->deltaTime = 1.0f / (float)app->fps;

  return true;
}

PNTR_APP_API void pntr_app_set_title(pntr_app* app, const char* title) {
  if (app == NULL) {
    return;
  }

  // TODO: update title

  app->title = title;
}


bool pntr_app_platform_set_size(pntr_app* app, int width, int height) {
    (void)width;
    (void)height;
    if (app == NULL || app->platform == NULL) {
        return false;
    }

    pntr_app_rgfw_platform* platform = (pntr_app_rgfw_platform*)app->platform;
    (void)platform;

    // TODO: do actual resize

    return true;
}


PNTR_APP_API void pntr_app_set_icon(pntr_app* app, pntr_image* icon) {
  (void)app;
  (void)icon;
  // TODO: set icon RGFW_window_setIcon(platform->window, icon, RGFW_AREA(3, 3), 4);
}

#endif  // PNTR_APP_RGFW_IMPLEMENTATION_ONCE
#endif  // PNTR_APP_IMPLEMENTATION && !PNTR_APP_HEADER_ONLY
#endif  // PNTR_APP_RGFW
