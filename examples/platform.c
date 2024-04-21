// #define MF_PLATFORM_USE_OPENGL
#pragma comment(lib, "GL")
#pragma comment(lib, "X11")
#define MF_PLATFORM_IMPLEMENTATION
#include "mf_platform.h"


int main()
{
    MFP_Platform platform = {};
    MFP_Init(&platform);
    MFP_WindowOpen(&platform, "Example", 0, 0, 1600, 900);

    bool running = true;
    while (running && platform.window.isOpen)
    {
        MFP_Begin(&platform);

        if (platform.input.keys['q'].pressed)
            running = false;

        MFP_End(&platform, true);
    }

    MFP_WindowClose(&platform);
    MFP_Destroy(&platform);
}
