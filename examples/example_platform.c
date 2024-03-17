// #define MF_PLATFORM_USE_OPENGL
#define MF_PLATFORM_IMPLEMENTATION
#include "mf_platform.h"


int main()
{
    MFP_Platform platform = {};
    mfp_init(&platform);
    mfp_window_open(&platform, "Example", 0, 0, 1600, 900);

    bool running = true;
    while (running && platform.window.isOpen)
    {
        mfp_begin(&platform);

        if (platform.input.keys['q'].pressed)
            running = false;

        mfp_end(&platform, true);
    }

    mfp_window_close(&platform);
    mfp_destroy(&platform);
}
