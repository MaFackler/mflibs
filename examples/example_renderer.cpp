#define MF_PLATFORM_USE_OPENGL
#define MF_PLATFORM_IMPLEMENTATION
#include "mf_platform.h"
#define MF_RENDERER_IMPLEMENTATION
#include "mf_renderer.h"

#define MF_OPENGL_IMPLEMENTATION
#include "mf_opengl.h"

#define MF_MATH_IMPLEMENTATION
#include "mf_math.h"

typedef mfm_v2 v2;
typedef mfm_v3 v3;

// F = G * (m1 * m2 / (r * r) )
//#define G_CONST 0.0000000000674f
#define G_CONST 10000.0f

struct Planet
{
    v2 pos;
    v2 velocity;
    v3 color;
    float radius, mass;
};


v2 calc_acceleration_to(Planet &p1, Planet &p2)
{
    v2 force_direction = mfm_v2_normalize(p2.pos - p1.pos);
    float distance_squared = mfm_v2_distance_squared(p2.pos, p1.pos);
    v2 force = force_direction * ((G_CONST * p1.mass * p2.mass) / distance_squared);
    v2 res = force / p1.mass;
    return res;
}

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

    Planet planets[2] = {0};
    planets[0].pos.x = 800;
    planets[0].pos.y = 450;
    planets[0].mass = 100;
    planets[0].radius = 100;
    planets[0].color = v3{1.0f, 1.0f, 0.0f};

    planets[1].pos.x = 1000;
    planets[1].pos.y = 450;
    planets[1].mass = 100;
    planets[1].radius = 30;
    planets[1].velocity = {0, 80.0f};
    planets[1].color = v3{0.3f, 0.3f, 0.3f};


    bool running = true;
    while (running && platform.window.isOpen)
    {
        mfp_begin(&platform);

        // Update
        if (platform.input.keys['q'].pressed)
            running = false;

        Planet &p1 = planets[0];
        Planet &p2 = planets[1];

        v2 acceleration = calc_acceleration_to(p2, p1);
        p2.velocity.x += acceleration.x * platform.timer.deltaSec;
        p2.velocity.y += acceleration.y * platform.timer.deltaSec;

        for (auto &p: planets)
        {
            p.pos.x += p.velocity.x * platform.timer.deltaSec;
            p.pos.y += p.velocity.y * platform.timer.deltaSec;
        }

        // Render
        // clear
        mfr_set_color(&renderer, 0.1f, 0.1f, 0.1f, 1.0f);
        mfr_push_clear(&renderer);

        // rectangle
        //mfr_set_offset(&renderer, 10.0f, 0.0f);
        //mfr_push_rect(&renderer, 10, 10, 30, 30);


        for (auto &p: planets)
        {
            mfr_set_color(&renderer, p.color.x, p.color.y, p.color.z, 1.0f);
            mfr_push_circle(&renderer, p.pos.x, p.pos.y, p.radius);
        }

        mfr_flush(&renderer);

        mfp_end(&platform);
    }

    mfp_window_close(&platform);
    mfp_destroy(&platform);
}
