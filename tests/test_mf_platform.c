#define MF_PLATFORM_USE_OPENGL
#define MF_IMPLEMENTATION
#include <mf.h>
#include <mf_test.h>
#include <mf_platform.h>
#pragma comment(lib, "X11")
#pragma comment(lib, "GL")

TEST(basic_window) {
    mfp_platform platform = {};
    mfp_init(&platform);
#if 0
    mfp_window_open(&platform, "Test", 0, 0, 800, 600);
    mfp_begin(&platform);
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    mfp_end(&platform, false);
    mf_sleep_ms(10);

    mfp_window_close(&platform);
#endif
    mfp_destroy(&platform);
}
