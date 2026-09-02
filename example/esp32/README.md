# pntr_app on the ESP32

Runs a pntr_app application on an ESP32 with an SPI display, using
[PlatformIO](https://platformio.org/).

The default board profile is the **ESP32-2432S028R "Cheap Yellow Display"** (CYD): a
320x240 ILI9341 panel with an XPT2046 resistive touch panel.

``` bash
pio run -e cyd -t upload -t monitor
```

## Screen size

pntr allocates the screen as a single contiguous RGBA8888 buffer, so a 320x240 screen needs
307,200 bytes. The CYD's ESP32-WROOM-32 has no PSRAM, and its largest contiguous DRAM block
is far below that, so a full resolution screen cannot be allocated.

Instead, pick a screen size that fits and let `pntr_app_esp32` upscale it by an integer
factor onto the panel, centred with a black border:

| Screen | Bytes | Scale on a 320x240 panel |
| ------ | ----- | ------------------------ |
| 320x240 | 307,200 | Will not allocate |
| 160x120 | 76,800 | 2x, fills the panel |
| 106x80 | 33,920 | 3x |
| 80x60 | 19,200 | 4x |

This example uses 160x120. If the screen is too large to allocate, `pntr_app_esp32_setup()`
reports the size it wanted and the largest block actually available, rather than failing
silently.

## Arduino entry point

Arduino-ESP32 defines `app_main()` itself and calls `setup()` and `loop()` from its own
task. Those have C++ linkage, so `src/main.cpp` is a small shim that forwards to
`pntr_app_esp32_setup()` and `pntr_app_esp32_loop()`. Under plain ESP-IDF the header
defines `app_main()` for you and no shim is needed.

## Another board

Set `PNTR_APP_ESP32_BOARD_CUSTOM` and define the pins yourself. The configuration macros are
listed at the top of [`pntr_app_esp32.h`](../../include/pntr_app_esp32.h).

``` ini
build_flags =
    -D PNTR_APP_ESP32
    -D PNTR_APP_ESP32_BOARD_CUSTOM
    -D PNTR_APP_ESP32_LCD_MOSI=13
    -D PNTR_APP_ESP32_LCD_SCLK=14
    ; ...
    -D PNTR_APP_ESP32_NO_TOUCH
```

## Not implemented yet

- Audio. The `PNTR_APP_LOAD_SOUND_FROM_MEMORY` and friends hooks are unset, so
  `pntr_load_sound()` returns `NULL` and playback is a no-op.
- Keyboard and gamepad input. Only touch, mapped to mouse events, is wired up.
- File loading. Build with `PNTR_NO_STDIO`; there is no SD card or filesystem support.
