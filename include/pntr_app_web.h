#ifdef EMSCRIPTEN
#include <emscripten.h>
#include <emscripten/html5.h>


// this is cheap/simple but magic-bytes is better
// https://github.com/konsumer/emscripten_browser_sound/blob/main/browser_sound.h#L50-L64

EM_JS(pntr_sound*, pntr_load_sound_from_memory, (const char* fileNamePtr, unsigned char* dataPtr, unsigned int dataSize), {
    const data = HEAPU8.slice(dataPtr, dataPtr + dataSize);
    const filename = UTF8ToString(fileNamePtr);

    let type = "application/octet-stream";

    if (filename.endsWith('.ogg')) {
        type='audio/ogg';
    }
    else if (filename.endsWith('.wav')) {
        type='audio/wav';
    }
    else {
        return 0;
    }

    const audio = new Audio();
    audio.src = URL.createObjectURL(new Blob([data], { type }));
    Module.pntr_sounds = Module.pntr_sounds || [];
    Module.pntr_sounds.push(audio);

    // Return the length instead of length - 1, as 0 or NULL is how to depict a failed load.
    return Module.pntr_sounds.length;
});

EM_JS(void, pntr_play_sound, (pntr_sound* sound, bool loop), {
    const audio = Module.pntr_sounds[sound - 1];
    if (!audio) {
        console.log('play: sound not loaded', {sound, pntr_sounds: Module.pntr_sound});
        return;
    }

    audio.loop = loop;
    let result = audio.play();

    if (result !== undefined) {
        // If it couldn't play the audio because of the autoplay policy, try to play it again after waiting a bit.
        // https://developer.mozilla.org/en-US/docs/Web/Media/Autoplay_guide#example_handling_play_failures
        result.catch((error) => {
            if (error.name === "NotAllowedError") {
                setTimeout(function() {
                    audio.pause();
                    audio.currentTime = 0;
                    pntr_play_sound(sound, loop);
                }, 500);
            }
        });
    }
})

EM_JS(void, pntr_stop_sound, (pntr_sound* sound), {
    const audio = Module.pntr_sounds[sound - 1];
    if (audio) {
        audio.pause();
        audio.currentTime = 0;
    }
})

EM_JS(void, pntr_unload_sound, (pntr_sound* sound), {
    const audio = Module.pntr_sounds[sound - 1];
    if (audio) {
        audio.pause();
        audio.currentTime = 0;
        revokeObjectURL(audio.src);
    }
})

EM_JS(void, pntr_app_init_js, (const char* title, int width, int height), {
    document.title = UTF8ToString(title);
    canvas.width = width;
    canvas.height = height;
    this.ctx = canvas.getContext('2d')
});

EM_JS(void, pntr_app_render_js, (void* bufferPtr, int width, int height), {
    const screen = new Uint8ClampedArray(HEAPU8.subarray(bufferPtr, bufferPtr + (width * height * 4)));
    this.ctx.putImageData(new ImageData(screen, width, height), 0, 0);
});


bool pntr_app_events(pntr_app* app) {
    return true;
}

bool pntr_app_render(pntr_app* app) {
    if (app == NULL || app->screen == NULL) {
        return false;
    }

    pntr_image* screen = app->screen;
    pntr_app_render_js((void*)screen->data, screen->width, screen->height);
    return true;
}

int pntr_app_emscripten_key(int eventType, const struct EmscriptenKeyboardEvent *keyEvent, void *userData) {
    pntr_app* app = (pntr_app*)userData;
    if (app == NULL || app->event == NULL) {
        return 0;
    }

    // Build the key event.
    pntr_app_event event;
    event.type = (eventType == EMSCRIPTEN_EVENT_KEYDOWN) ? PNTR_APP_EVENTTYPE_KEY_DOWN : PNTR_APP_EVENTTYPE_KEY_UP;

    // TODO: keyCode is deprecated, so do some string checkings?
    event.key = keyEvent->keyCode;
    if (event.key <= 0) {
        return 0;
    }

    // Invoke the event
    pntr_app_process_event(app, &event);

    // Return false as we're taking over the event.
    return 1;
}

bool pntr_app_init(pntr_app* app) {
    if (app == NULL) {
        return false;
    }
    pntr_app_init_js(app->title, app->width, app->height);

    // Keyboard
    emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, app, true, pntr_app_emscripten_key);
    emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, app, true, pntr_app_emscripten_key);

    return true;
}

void pntr_app_close(pntr_app* app) {
}

bool pntr_app_platform_update_delta_time(pntr_app* app) {
    return true;
}

void pntr_app_set_title(pntr_app* app, const char* title) {
    pntr_app_init_js(app->title, app->width, app->height);
}

bool _pntr_app_platform_set_size(pntr_app* app, int width, int height) {
    pntr_app_init_js(app->title, app->width, app->height);
    return true;
}

#endif
