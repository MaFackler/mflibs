
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "mf.h"
#define MF_MATH_IMPLEMENTATION
#include "mf_math.h"
#define MF_VECTOR_IMPLEMENTATION
#include "mf_vector.h"
#define MF_FONT_IMPLEMENTATION
#include "mf_font.h"

#define MF_PLATFORM_USE_OPENGL
#define MF_PLATFORM_IMPLEMENTATION
#include "mf_platform.h"

#define MF_OPENGL_IMPLEMENTATION
#include "mf_opengl.h"


const char *VS_SRC = R"(
#version 330 core
layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 texCoords;
out vec2 TexCoords;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(pos.xy, 0.0, 1.0);
    TexCoords = texCoords;
})";

const char *FS_SRC = R"(
#version 330 core
in vec2 TexCoords;
uniform sampler2D text;

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    //gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0) * sampled;
})";


struct v2 {
    unsigned int x;
    unsigned int y;
};

struct Character {
    u32 texture_id;
    v2 size;
    v2 bearing;
    u32 advance;
};

static Character characters[256] = {0};


void render_text(mffo_font *font, u32 *tids, mf_vec_float *vertices, u32 vbo, u32 ebo, const char *text, float x, float y)
{
    char c = 0;
    mf_vec_clear(*vertices);
    float xc = x;
    float yc = y;
    while ((c = *text++) != 0) {
        mffo_font_char *ch = &font->characters[c];
        float xmin = xc + ch->xbearing;
        float xmax = xmin + ch->width;
        //float ymin = yc + ch->size.y - ch->size.y;
        //float ymin = yc + ch->size.y + ch->bearing.y;
        float ymin = y - (float) ch->height + (float) ch->ybearing;
        float ymax = ymin + ch->height;

        *mf_vec_add(*vertices) = xmin;
        *mf_vec_add(*vertices) = ymin;
        *mf_vec_add(*vertices) = 0.0f;
        *mf_vec_add(*vertices) = 1.0f;

        *mf_vec_add(*vertices) = xmax;
        *mf_vec_add(*vertices) = ymin;
        *mf_vec_add(*vertices) = 1.0f;
        *mf_vec_add(*vertices) = 1.0f;

        *mf_vec_add(*vertices) = xmax;
        *mf_vec_add(*vertices) = ymax;
        *mf_vec_add(*vertices) = 1.0f;
        *mf_vec_add(*vertices) = 0.0f;

        *mf_vec_add(*vertices) = xmin;
        *mf_vec_add(*vertices) = ymax;
        *mf_vec_add(*vertices) = 0.0f;
        *mf_vec_add(*vertices) = 0.0f;

        mfgl_texture_bind(tids[c]);
        mfgl_vertex_buffer_bind(vbo);
        mfgl_error_check();
        glBufferSubData(GL_ARRAY_BUFFER, 0, mf_vec_size(*vertices) * sizeof(float), *vertices);
        //glBindBuffer(GL_ARRAY_BUFFER, 0);
        mfgl_element_buffer_bind(ebo);
        mfgl_element_buffer_draw(ebo, 6); 

        xc += (ch->advance >> 6);
        mf_vec_clear(*vertices);
    }
}


void load_texture(mffo_font_char *c, u32 *tid) {
    if (c->data == NULL)
        return;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, tid);
    glBindTexture(GL_TEXTURE_2D, *tid);
    glTexImage2D(
                 GL_TEXTURE_2D,
                 0,
                 GL_RED,
                 c->width,
                 c->height,
                 0,
                 GL_RED,
                 GL_UNSIGNED_BYTE,
                 c->data
                );
    // set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

int main() {


    mffo_font font = {};
    mffo_font_init(&font, "/usr/share/fonts/TTF/Hack-Bold.ttf", 48.0f);

    mfp_platform platform = {};
    mfp_init(&platform);
    mfp_window_open(&platform, "example", 0, 0, 800, 600);

	u32 vs = mfgl_shader_vertex_create(VS_SRC);
	u32 fs = mfgl_shader_fragment_create(FS_SRC);
    u32 program = mfgl_shader_program_create(vs, fs);
    mfgl_shader_program_use(program);

    //auto projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
	mfm_m4 projection = mfm_m4_ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.0f, 100.0f);	
    u32 location_projection = mfgl_shader_uniform_location(program, "projection");
    mfgl_shader_uniform_4fv(location_projection, 1, (float *) &projection.m[0]);

    //u32 vao = mfgl_vertex_array_create();
    u32 vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 4, NULL, GL_DYNAMIC_DRAW);
    mf_vec_float vertices = NULL;

    u32 ebo = 0;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    u32 indices[] = {
        0, 1, 2,
        0, 2, 3,
    };
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_DYNAMIC_DRAW);

    mfgl_vertex_buffer_bind(vbo);
    mfgl_element_buffer_bind(ebo);
    mfgl_vertex_attrib_link(0, 2, 0, 4);
    mfgl_vertex_attrib_link(1, 2, 2, 4);

    mfgl_blend(true);

    u32 textures[256] = {0};

    for (char i = 0; i < MF_ArrayLength(textures); ++i) {
        load_texture(&font.characters[i], &textures[i]);
    }

    bool running = true;
    while (running && platform.window.isOpen)
    {
        mfp_begin(&platform);

        if (platform.input.keys['q'].pressed) {
            running = false;
        }

        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        render_text(&font, &textures[0], &vertices, vbo, ebo, "Zeile Eins", 0, -font.descent + font.linegap);
        render_text(&font, &textures[0], &vertices, vbo, ebo, "Zeile 2", 0, -font.descent);

        mfp_end(&platform);
    }

    mfp_window_close(&platform);
    mfp_destroy(&platform);
}
