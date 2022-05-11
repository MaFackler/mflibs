#define MF_PLATFORM_USE_OPENGL
#define MF_PLATFORM_IMPLEMENTATION
#include "mf_platform.h"

int main() {
    mfp_platform p = {};

    const int width = 1280;
    const int height = 720;
    const char *title = "Craft";
    mfp_init(&p);
    mfp_window_open(&p, title, 0, 0, width, height);
    glViewport(0, 0, width, height);

    bool running = true;
    while (running && p.window.isOpen) {
        mfp_begin(&p);

        if (p.input.keys[MF_KEY_ESCAPE].pressed) {
            running = false;
        }

        if (p.input.keys['1'].pressed) {
            mfp_window_toggle_fullscreen(&p);
        }

        if (p.input.mouseX > 50) {
            //printf("Mouse is greather that 50.\n");
        }

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);



        mfp_end(&p, true);
    }

    mfp_window_close(&p);
    mfp_destroy(&p);
}
