#include "mf.h"
#define MF_PLATFORM_USE_OPENGL
#define MF_PLATFORM_IMPLEMENTATION
#include "mf_platform.h"
#define MF_RENDERER_IMPLEMENTATION
#include "mf_renderer.h"

#define MF_OPENGL_IMPLEMENTATION
#include "mf_opengl.h"
#include "mf_opengl2.h"

#define MF_MATH_IMPLEMENTATION
#include "mf_math.h"

#define MF_FONT_IMPLEMENTATION
#include "mf_font.h"

#define MF_GUI_IMPLEMENTATION
#include "mf_gui.h"


typedef mfm_v2 v2;
typedef mfm_v3<float> v3;

// F = G * (m1 * m2 / (r * r) )
//#define G_CONST 0.0000000000674f
#define G_CONST 10000.0f

#if 0
static const char* SRC_VS = 
"#version 330 core\n"
"layout (location = 0) in vec3 pos;\n"
"layout (location = 1) in vec3 color;\n"
"layout (location = 2) in vec2 tex_coord;\n"
"out vec3 both_color;\n"
"out vec2 both_texCoord;\n"
"void main() {\n"
"    gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);\n"
"    both_color = color;\n"
"    both_texCoord = tex_coord;\n"
"}\0";

static const char* SRC_FS = 
"#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 both_color;\n"
"in vec2 both_texCoord;\n"
"uniform sampler2D sampler;\n"
"void main() {\n"
"    //FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"    //FragColor = vec4(both_color, 1.0f);\n"
"    FragColor = texture(sampler, both_texCoord) * vec4(both_color, 1.0);\n"
"    //FragColor = texture(sampler, both_texCoord);\n"
"    //FragColor = vec4(texture2D(sampler, both_texCoord.st, 0.0).w * both_color, 1.0);\n"
"    //FragColor = vec4(both_texCoord.x, both_texCoord.y, 0, 0);\n"
"}\0";
#endif

static const char *SRC_VS =
"#version 330 core\n"
"layout (location = 0) in vec2 pos;\n"
"layout (location = 1) in vec3 color;\n"
"layout (location = 2) in vec2 tex;\n"
"out vec3 color_;\n"
"out vec2 tex_;\n"
"void main() {\n"
"   gl_Position = vec4(pos.x, pos.y, 0, 1.0);\n"
"   color_ = color;\n"
"   tex_ = tex;\n"
"}\n";

static const char *SRC_FS = 
"#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 color_;\n"
"in vec2 tex_;\n"
"uniform sampler2D sampler;\n"
"void main() {\n"
//"   FragColor = texture(sampler, tex_) * vec4(color_.r, color_.g, color_.b, 1.0);\n"
"   FragColor = texture(sampler, tex_);\n"
//"   FragColor = vec4(color_.r, color_.g, color_.b, 1.0);\n"
"}\n";

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

void my_render_bitmap(unsigned int bitmap_id, float x, float y, float w, float h)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    mfgl_texture_bind(bitmap_id);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(x, y + h);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(x + w, y + h);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(x + w, y);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(x, y);
    glEnd();
    mfgl_texture_bind(0);
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
}

void render_text(mffo_font *font, const char *text) {

    float x = 0;
    for (int i = 0; i < strlen(text); ++i) {
        auto ch = font->characters[text[i]];
        glBegin(GL_QUADS);
            glTexCoord2f(ch.u0, ch.v0);
            glVertex2f(x + ch.xbearing, ch.ybearing);
            //glVertex2f(-1.0f, -1.0f);
            glTexCoord2f(ch.u1, ch.v0);
            glVertex2f(x + ch.xbearing + ch.width, ch.ybearing);
            //glVertex2f(1.0f, -1.0f);
            glTexCoord2f(ch.u1, ch.v1);
            glVertex2f(x + ch.xbearing + ch.width, ch.ybearing + ch.height);
            //glVertex2f(1.0f, 1.0f);
            glTexCoord2f(ch.u0, ch.v1);
            glVertex2f(x + ch.xbearing, ch.ybearing + ch.height);
            //glVertex2f(-1.0f, 1.0f);
        glEnd();
        x += ch.advance;
    }
}

int main() {
    u32 width = 1600;
    u32 height = 900;
    mfp_platform platform = {};
    mfp_init(&platform);
    mfp_window_open(&platform, "Example-Renderer", 0, 0, width, height);
    mfgl_viewport_bottom_up(width, height);

#if 0
    mfr_renderer renderer = {};
    renderer.set_color = mfgl_set_color;
    renderer.render_rect = mfgl_draw_rect;
    renderer.render_clear = mfgl_clear;
    renderer.render_circle = mfgl_draw_circle;
    renderer.render_bitmap = my_render_bitmap;
    mfr_init(&renderer, 1024 * 1024 * 1024);

    glEnable(GL_TEXTURE_2D);

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
#endif

    mffo_font font;
#ifdef _WIN32
    const char *path = "c:/windows/fonts/arialbd.ttf";
#else
    const char *path = "/usr/share/fonts/ubuntu/Ubuntu-B.ttf";
#endif
    mffo_font_init(&font, path, 128.0f);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    u32 texture_font = mfgl_texture_create_argb(FONT_ATLAS_DIM, FONT_ATLAS_DIM, font.data);
#if 0
    unsigned int texture_font;
    glGenTextures(1, &texture_font);
    glBindTexture(GL_TEXTURE_2D, texture_font);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
                 FONT_ATLAS_DIM, FONT_ATLAS_DIM, 0, GL_BGRA,
                 GL_UNSIGNED_BYTE, font.data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
#endif

#if 0
    u32 vs = mfgl_shader_vertex_create(SRC_VS);
    u32 fs = mfgl_shader_fragment_create(SRC_FS);
    u32 program = mfgl_shader_program_create(vs, fs);
    mfgl_shader_delete(vs);
    mfgl_shader_delete(fs);
    mfgl_shader_program_use(program);
    mfgl_error_check();

    u32 location_color = mfgl_shader_uniform_location(program, "color");
    mfgl_shader_uniform_4f(location_color, 1.0f, 0.0f, 0.0f, 0.0f);

    //u32 location_smapler = mfgl_shader_uniform_location(program, "sampler");
    //mfgl_shader_uniform_1i(location_smapler, 0);

    mfg_ui ui;
    mfg_init(&ui, 1600, 900);
    
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
    u32 texture_id = mfgl_create_texture_argb(512, 512, font.data);

	float vertices[] = {
		 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		 0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f, 0.5f, 1.0f,
	};    

	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 2,   // first triangle
	};
#endif

#if 0
    float *data = NULL;
    unsigned int *indices = NULL;
    u32 vbo = mfgl_vertex_buffer_dynamic_create(NULL, 512, &data);
    //u32 vbo = mfgl_vertex_buffer_create(&vertices[0], MF_ArrayLength(vertices));
    //u32 vao = mfgl_vertex_array_create();
    //u32 ebo = mfgl_element_buffer_create(&ui.indices[0], MF_ArrayLength(ui.indices));
    u32 ebo = mfgl_element_buffer_dynamic_create(NULL, 512, &indices);

    //mfgl_vertex_array_bind(vao);
    mfgl_element_buffer_bind(ebo);
    mfgl_vertex_buffer_bind(vbo);
    mfgl_vertex_attrib_link(0, 2, 0, 7); // x,y
    mfgl_vertex_attrib_link(1, 3, 2, 7); // color r.g.b
    mfgl_vertex_attrib_link(2, 2, 5, 7); // u,t
#endif

    mfgl_error_check();
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


#if 0
        for (auto &p: planets)
        {
            mfr_set_color(&renderer, p.color.x, p.color.y, p.color.z, 1.0f);
            mfr_push_circle(&renderer, p.pos.x, p.pos.y, p.radius);
        }
#endif

        mfr_push_bitmap(&renderer, texture_font, 0, 0, 512, 512);

        
        mfr_flush(&renderer);

#if 0
        mfgl_viewport_top_down(width, height);
        mfgl_texture_bind(texture_font);
#endif
        //mffo_charrect crect;
        //mffo_font_get_charrect(&font, &crect, 'c');
        //mfgl_draw_rect(0, 0, 512, 512);
        //mfui_button(&font);
#endif
        if (platform.input.keys['q'].pressed)
            running = false;

        //if (platform.input.keys['s'].pressed)
        //    data[0] = 0.0f;

#if 0
        // UI
        mfg_begin(&ui);

        mfg_button(&ui, 100, 100, 512, 512);
        

        mfgl_shader_program_use(program);
        //mfgl_vertex_array_bind(vao);
        mfgl_set_color(1.0f, 0.0f, 0.0f, 1.0f);
        mfgl_clear();

        //mfgl_vertex_array_bind(vao);
        mfgl_texture_bind(texture_font);
        //mfgl_element_buffer_draw(ebo, 6);

        memcpy((unsigned char *) data, &ui.vertices[0], sizeof(mfg_vertex) * ui.vertices_index);
        memcpy((unsigned char *) &indices[0], &ui.indices[0], sizeof(unsigned int) * ui.indices_index);

        mfgl_element_buffer_bind(ebo);
        mfgl_vertex_buffer_bind(vbo);
        //mfgl_vertex_buffer_draw(vbo, 3);
        mfgl_element_buffer_draw(ebo, ui.indices_index);
        
        mfg_end(&ui);
        //glDisable(GL_BLEND);


        mffo_charrect charrect;
        mffo_font_get_charrect(&font, &charrect, 'c');
#endif

        glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindTexture(GL_TEXTURE_2D, texture_font);

        auto ch = font.characters['c'];
#if 0
        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f);
            glVertex2f(-1.0f, -1.0f);
            glTexCoord2f(1.0f, 0.0f);
            glVertex2f(1.0f, -1.0f);
            glTexCoord2f(1.0f, 1.0f);
            glVertex2f(1.0f, 1.0f);
            glTexCoord2f(0.0f, 1.0f);
            glVertex2f(-1.0f, 1.0f);
        glEnd();
#else
#if 0
        glBegin(GL_QUADS);
            glTexCoord2f(ch.u0, ch.v0);
            glVertex2f(ch.xbearing, ch.ybearing);
            //glVertex2f(-1.0f, -1.0f);
            glTexCoord2f(ch.u1, ch.v0);
            glVertex2f(ch.xbearing + ch.width, ch.ybearing);
            //glVertex2f(1.0f, -1.0f);
            glTexCoord2f(ch.u1, ch.v1);
            glVertex2f(ch.xbearing + ch.width, ch.ybearing + ch.height);
            //glVertex2f(1.0f, 1.0f);
            glTexCoord2f(ch.u0, ch.v1);
            glVertex2f(ch.xbearing, ch.ybearing + ch.height);
            //glVertex2f(-1.0f, 1.0f);
        glEnd();
#endif

        render_text(&font, "Maximilian");
#endif
        mfgl_error_check();
        mfp_end(&platform);
        mf_sleep_ms(16);
    }

    mfp_window_close(&platform);
    mfp_destroy(&platform);
}
