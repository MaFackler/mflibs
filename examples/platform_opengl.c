#pragma comment(lib, "GL")
#pragma comment(lib, "X11")
#define MF_IMPLEMENTATION
#include <mf_platform.h>
#include <mf_platform_opengl.h>

int main(int argc, char **argv) {
    MFP_Platform platform = {0};
    MFP_Init(&platform);
    // TODO: this is not clear. Maybe something like attach to window?
    MFP_WindowOpen(&platform, "Example-Opengl", 0, 0, 1600, 900);
    MFP_InitOpengl(&platform);
    bool running = true;
    while (running && platform.window.isOpen) {
        MFP_Begin(&platform);
        MFP_BeginOpengl(&platform);

        if (platform.input.keys['q'].pressed) {
            running = false;
        }

        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        MFP_EndOpengl(&platform);
        MFP_End(&platform, true);
    }
    MFP_WindowClose(&platform);
    MFP_Destroy(&platform);
}
