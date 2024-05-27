#include <mf_lib.h>
MF_LIB_LINUX("GL")
MF_LIB_LINUX("X11")
MF_LIB_LINUX("m")
MF_LIB_WINDOWS("opengl32")
MF_LIB_WINDOWS("gdi32")

#define MF_IMPLEMENTATION
#include <mf_platform.h>
#include <mf_platform_opengl.h>

#include <time.h>

int main(int argc, char **argv) {
    MFP_Platform platform = {0};
    MFP_SetTargetFps(&platform, 60);
    MFP_Init(&platform);
    MFP_InitOpengl(&platform);

    MFP_WindowOpen(&platform, "Window",
                   MFP_WINDOWPOS_CENTER, MFP_WINDOWPOS_CENTER,
                   800, 600);

    while (!MFP_WindowShouldClose(&platform)) {
        MFP_Begin(&platform);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        MFP_End(&platform, true);
    }
}
