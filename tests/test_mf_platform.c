#define MF_PLATFORM_USE_OPENGL
#define MF_IMPLEMENTATION
#include <mf.h>
#include <mf_test.h>
#include <mf_platform.h>
#include <mf_platform_opengl.h>
#pragma comment(lib, "X11")
#pragma comment(lib, "GL")

TEST(MFP_Platform) {
    MFP_Platform platform = {};
    MFP_Init(&platform);
#if 0
    MFP_WindowOpen(&platform, "Test", 0, 0, 800, 600);
    MFP_InitOpengl(&platform);

    MFP_Begin(&platform);
    MFP_BeginOpengl(&platform);

    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    MFP_EndOpengl(&platform);
    MFP_End(&platform, false);

    MFP_WindowClose(&platform);
#endif
    MFP_Destroy(&platform);
}
