#include "mf_lib.h"
MF_LIB_LINUX("X11")
MF_LIB_LINUX("m")
MF_LIB_WINDOWS("gdi32")
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
