#define MF_PLATFORM_USE_OPENGL
#define MF_PLATFORM_IMPLEMENTATION
#include "mf_platform.h"
#define MF_RENDERER_IMPLEMENTATION
#include "mf_renderer.h"

#define MF_OPENGL_IMPLEMENTATION
#include "mf_opengl.h"


int main()
{
    u32 width = 1600;
    u32 height = 900;
    mfp_platform platform = {};
    mfp_init(&platform);
    mfp_window_open(&platform, "Example-Renderer", 0, 0, width, height);

    mfr_renderer renderer = {};
    renderer.set_color = mfgl_set_color;
    renderer.render_rect = mfgl_draw_rect;
    renderer.render_clear = mfgl_clear;
    renderer.render_circle = mfgl_draw_circle;
    mfr_init(&renderer, 1024 * 1024 * 1024);

    mfgl_viewport_bottom_up(width, height);


    bool running = true;
    while (running && platform.window.isOpen)
    {
        mfp_begin(&platform);

        if (platform.input.keys['q'].pressed)
            running = false;

        // clear
        mfr_set_color(&renderer, 1.0f, 0.0f, 0.0f, 1.0f);
        mfr_push_clear(&renderer);

        // rectangle
        mfr_set_color(&renderer, 1.0f, 1.0f, 0.0f, 1.0f);
        //mfr_set_offset(&renderer, 10.0f, 0.0f);
        //mfr_push_rect(&renderer, 10, 10, 30, 30);
        mfr_push_circle(&renderer, 100, 100, 100);

        mfr_flush(&renderer);

        mfp_end(&platform);
    }

    mfp_window_close(&platform);
    mfp_destroy(&platform);
}
