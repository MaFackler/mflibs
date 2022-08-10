// TODO: dont use std::vector
#include <vector>
#include "mf.h"
#define MF_PLATFORM_IMPLEMENTATION
#include <mf_platform/platform.h>
#include <mf_platform/graphics/opengl.h>

#define MF_OPENGL_IMPLEMENTATION
#include "mf_opengl.h"

#define MF_MATH_IMPLEMENTATION
#include "mf_math.h"
#define MF_VECTOR_IMPLEMENTATION
#include "mf_vector.h"
#define MF_TIME_IMPLEMENTATION
#include "mf_time.h"
#define MF_RENDERER_IMPLEMENTATION
#include "mf_renderer.h"
#define MF_FONT_IMPLEMENTATION
#include "mf_font.h"

typedef mf::math::v2<f32> v2;
typedef mf::math::v3<f32> v3;
typedef mf::math::v4<f32> v4;
using mf::math::m4;
using mf::renderer::Renderer2D;
using namespace mf::gl;
using namespace mf::platform;

struct IExample {
    virtual void init(IPlatform &p, Renderer2D &r) = 0;
    virtual void update(IPlatform &p) = 0;
    virtual void render(IPlatform &p, Renderer2D &r) = 0;
    virtual void shutdown(IPlatform &p) = 0;
};

// TODO: only used by craft?
struct Vertex {
    v3 pos;
    v4 color;
};
#include "example_craft.cpp"
#include "example_tetris.cpp"
#include "example_planets.cpp"


int main() {

    //mf::Font font;
    //mf::font_init(&font, "foo", 7.0f);
    Platform p = {};

    const int width = 1600;
    const int height = 900;
    const char *title = "Examples 2D";

    p.init();
    p.window_create(title, 0, 0, width, height);
    opengl_init(p);
    //glViewport(0, 0, width, height);
    Renderer2D r = {};
    r.init(MF_MegaByte(64));
    r.func_set_color = mf::gl::set_color;
    r.func_render_rect = mf::gl::draw_rect;
    r.func_render_circle = mf::gl::draw_circle;
    r.func_render_clear = mf::gl::clear;
    
    // TODO: include craft example 2D vs 3D
    // example_craft::ExampleCraft craft;
    example_tetris::ExampleTetris tetris;
    example_planets::ExamplePlanets planets;

    std::vector<IExample*> examples;
    // examples.push_back(&craft);  TODO: cannot switch back and forth between shader and immediate mode
    examples.push_back(&tetris);
    examples.push_back(&planets);

    int example_index = 1;
    examples[example_index]->init(p, r);

    bool running = true;
    while (running && p.window.isOpen) {
        p.begin();

        if (p.input.keys['2'].pressed) {
            examples[example_index]->shutdown(p);
            example_index++;
            if (example_index >= examples.size()) {
                example_index = 0;
            }
            examples[example_index]->init(p, r);
        }
        if (p.input.keys[MF_KEY_ESCAPE].pressed) {
            running = false;
        }

        examples[example_index]->update(p);

        examples[example_index]->render(p, r);
        r.flush();
        p.end();
        mf::time::sleep_ms(16);
    }

    p.window_close();
    p.destroy();
}
