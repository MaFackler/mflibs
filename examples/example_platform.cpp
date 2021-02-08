#define MF_PLATFORM_USE_OPENGL
#define MF_PLATFORM_IMPLEMENTATION
#include "mf_platform.h"


int main()
{
    mfp_platform platform = {};
    mfp_init(&platform);
    mfp_window_open(&platform, "Example", 0, 0, 1600, 900);

    bool running = true;
    while (running && platform.window.isOpen)
    {
        mfp_begin(&platform);

        if (platform.input.keys['q'].pressed)
            running = false;

        glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        mfp_end(&platform);
    }

    mfp_window_close(&platform);
    mfp_destroy(&platform);
}
