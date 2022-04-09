#include "mf.h"
#define MF_PLATFORM_USE_OPENGL
#define MF_PLATFORM_IMPLEMENTATION
#include "mf_platform.h"
#define MF_RENDERER_IMPLEMENTATION
#include "mf_renderer.h"

#define MF_OPENGL_IMPLEMENTATION
#include "mf_opengl.h"

#define MF_MATH_IMPLEMENTATION
#include "mf_math.h"

#define MF_FONT_IMPLEMENTATION
#include "mf_font.h"


typedef mfm_v2 v2;
typedef mfm_v3 v3;

// F = G * (m1 * m2 / (r * r) )
//#define G_CONST 0.0000000000674f
#define G_CONST 10000.0f

static const char* SRC_VS = 
"#version 330 core\n"
"layout (location = 0) in vec3 pos;\n"
"void main() {\n"
"    gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);\n"
"}\0";

static const char* SRC_FS = 
"#version 330 core\n"
"out vec4 FragColor;"
"void main() {\n"
"    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\0";

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

void my_draw_text(mffo_font *font, float x, float y, const char *text)
{
   // assume orthographic projection with units = screen pixels, origin at top left
    
    mfgl_texture(true);
    mfgl_blend(true);

    glBegin(GL_QUADS);
    while (*text) {
        if (*text >= 32 && *text < 128) {
            stbtt_aligned_quad q;
            stbtt_GetBakedQuad(font->chardata, 512,512, *text-32, &x,&y,&q,1);//1=opengl & d3d10+,0=d3d9
            glTexCoord2f(q.s0,q.t0); glVertex2f(q.x0,q.y0);
            glTexCoord2f(q.s1,q.t0); glVertex2f(q.x1,q.y0);
            glTexCoord2f(q.s1,q.t1); glVertex2f(q.x1,q.y1);
            glTexCoord2f(q.s0,q.t1); glVertex2f(q.x0,q.y1);
        }
        ++text;
    }
    glEnd();
    mfgl_texture(false); 
    mfgl_blend(false); 
}

void my_render_bitmap(unsigned int bitmap_id, float x, float y, float w, float h)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    mfgl_bind_texture(bitmap_id);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(x, y + h);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(x + w, y + h);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(x + w, y);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(x, y);
    glEnd();
    mfgl_bind_texture(0);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

void mfui_button(mffo_font * font)
{
    int width = 100;
    int height = 50;
    int margin = 15;
    mfgl_set_color(1.0f, 1.0f, 1.0f, 1.0f);
    mfgl_draw_rect(0, 0, width, height);
    mfgl_set_color(0.0f, 0.0f, 0.0f, 1.0f);
    my_draw_text(font, 0, height - margin, "Button");
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
    renderer.render_bitmap = my_render_bitmap;
    mfr_init(&renderer, 1024 * 1024 * 1024);

    glEnable(GL_TEXTURE_2D);
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

    mffo_font font;
    mffo_font_alloc(&font, "/usr/share/fonts/ubuntu/Ubuntu-B.ttf");
    u32 texture_font = mfgl_create_texture_alpha(font.dim, font.dim, font.data);

    u32 vs = mfgl_shader_vertex_create(SRC_VS);
    u32 fs = mfgl_shader_fragment_create(SRC_FS);
    u32 program = mfgl_shader_program_create(vs, fs);
    mfgl_shader_delete(vs);
    mfgl_shader_delete(fs);
    mfgl_shader_program_use(program);
    mfgl_error_check();
    
    u32 *pixels = (u32 *) malloc(512 * 512 * sizeof(u32));
    for (size_t y = 0; y < 512; ++y)
    {
        for (size_t x = 0; x < 512; ++x)
        {
            u32 color = 0xFFFF0000;
            if (x < 100)
            {
                color = 0xFF00FF00;
            }
            if (y > 400)
            {
                color = 0xFF0000FF;
            }
            pixels[y * 512 + x] = color;
        }
    }
    u32 texture_id = mfgl_create_texture_argb(512, 512, (u32 *) font.data);

	float vertices[] = {
		 0.5f,  0.5f, 0.0f,  // top right
		 0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left 
	};
	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};



    u32 vbo = mfgl_vertex_buffer_create(&vertices[0], MF_ArrayLength(vertices));
    u32 vao = mfgl_vertex_array_create();
    u32 ebo = mfgl_element_buffer_create(&indices[0], MF_ArrayLength(indices));


    mfgl_vertex_array_bind(vao);
    mfgl_element_buffer_bind(ebo);
    mfgl_vertex_buffer_bind(vbo);
    mfgl_vertex_attrib_link(0, 3);

    mfgl_wireframe(false);

    bool running = true;
    while (running && platform.window.isOpen)
    {
        mfp_begin(&platform);
#if 0
        mfgl_viewport_bottom_up(width, height);

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

        mfr_push_bitmap(&renderer, texture_font, 0, 0, 512, 512);

        
        mfr_flush(&renderer);

        mfgl_viewport_top_down(width, height);
        mfgl_bind_texture(texture_font);
        mffo_charrect crect;
        mffo_font_get_charrect(&font, &crect, 'c');
        //mfgl_draw_rect(0, 0, 512, 512);
        mfui_button(&font);
        my_draw_text(&font, 20, 100, "hello");
#endif
        if (platform.input.keys['q'].pressed)
            running = false;

        mfgl_shader_program_use(program);
        mfgl_vertex_array_bind(vao);
        mfgl_clear();
        //mfgl_vertex_buffer_draw(vbo, 3);
        mfgl_element_buffer_draw(ebo, 6);


        mfp_end(&platform);
    }

    mfp_window_close(&platform);
    mfp_destroy(&platform);
}
