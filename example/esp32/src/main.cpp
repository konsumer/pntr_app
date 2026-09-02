// Arduino-ESP32 owns app_main() and calls setup()/loop() from its own task. Both have C++
// linkage, so this shim hands control to the C application in pntr_app_example_esp32.c.

extern "C" bool pntr_app_esp32_setup(void);
extern "C" bool pntr_app_esp32_loop(void);

static bool running = false;

void setup() {
    running = pntr_app_esp32_setup();
}

void loop() {
    if (running) {
        running = pntr_app_esp32_loop();
    }
}
