#define MF_IMPLEMENTATION
#include <mf_platform.h>
#include <mf_platform_opengl.h>

int main(int argc, char **argv) {
    MFP_Platform platform = {0};
    mfp_init(&platform);
    // TODO: this is not clear. Maybe something like attach to window?
    mfp_window_open(&platform, "Example-Opengl", 0, 0, 1600, 900);
    mfp_init_opengl(&platform);
    bool running = true;
    while (running && platform.window.isOpen) {
        mfp_begin(&platform);
        mfp_begin_opengl(&platform);

        if (platform.input.keys['q'].pressed) {
            running = false;
        }

        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        mfp_end_opengl(&platform);
        mfp_end(&platform, true);
    }
    mfp_window_close(&platform);
    mfp_destroy(&platform);
}
