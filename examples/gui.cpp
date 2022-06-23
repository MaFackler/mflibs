#include <ft2build.h>
#include FT_FREETYPE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "mf.h"
#define MF_MATH_IMPLEMENTATION
#include "mf_math.h"
#define MF_VECTOR_IMPLEMENTATION
#include "mf_vector.h"

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

void load_chars(FT_Face &face) {

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            continue;
        }
        // generate texture
        unsigned int texture = 0;
        glGenTextures(1, &texture);
        mfgl_error_check();
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
                     GL_TEXTURE_2D,
                     0,
                     GL_RED,
                     face->glyph->bitmap.width,
                     face->glyph->bitmap.rows,
                     0,
                     GL_RED,
                     GL_UNSIGNED_BYTE,
                     face->glyph->bitmap.buffer
                    );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
            texture, 
            {(u32) face->glyph->bitmap.width, (u32) face->glyph->bitmap.rows},
            {(u32) face->glyph->bitmap_left, (u32) face->glyph->bitmap_top},
            (u32) face->glyph->advance.x
        };
        characters[c] = character;
    }
    mfgl_error_check();

}

void render_text(const char *text, float x, float y, float scale, u32 vao, u32 vbo)
{

    glBindVertexArray(vao);
    glActiveTexture(GL_TEXTURE0);

    // iterate through all characters
    char c = 0;
    while ((c = *text++) != '\0') {
        Character ch = characters[c];

        float xpos = x + ch.bearing.x * scale;
        float ypos = y - (ch.size.y - ch.bearing.y) * scale;

        float w = ch.size.x * scale;
        float h = ch.size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.texture_id);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

int main() {

    // Freetype
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        return -1;
    }

    FT_Face face;
    if (FT_New_Face(ft, "/usr/share/fonts/TTF/FiraSans-Bold.ttf", 0, &face)) {
        return -1;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);
    if (FT_Load_Char(face, 'X', FT_LOAD_RENDER)) {
        return -1;
    }



    mfp_platform platform = {};
    mfp_init(&platform);
    mfp_window_open(&platform, "Example", 0, 0, 800, 600);
    //glEnable(GL_TEXTURE_2D);
    load_chars(face);

#if 0
    u32 vao = mfgl_vertex_array_create();
    mfgl_vertex_buffer_bind(vao);
    float *vertices = NULL;


    u32 vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    mfgl_vertex_buffer_bind(vbo);
    mfgl_vertex_attrib_link(0, 4, 0, 4);


    u32 location_text_color = mfgl_shader_uniform_location(program, "textColor");
    glUniform3f(location_text_color, 1.0f, 0.0f, 0.0f);

	//mfm_m4 projection = mfm_m4_ortho(0.0f, 1600.0f, 0.0f, 900.0f, 0.0f, 100.0f);	
    auto projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
    u32 location_projection = mfgl_shader_uniform_location(program, "projection");
    mfgl_shader_uniform_4fv(location_projection, 1, (float *) &projection[0]);
#endif

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

#if 0
    float vertices[] = {
        0.0f, 0.0f, 0.0f, 0.0f,
        100.0f, 0.0f, 1.0f, 0.0f,
        100.0f, 100.0f, 1.0f, 1.0f,
        0.0f, 100.0f, 0.0f, 1.0f
    };
    u32 vbo = mfgl_vertex_buffer_create(vertices, MF_ArrayLength(vertices));
#endif
#if 0 
    u32 indices[] = {
        0, 1, 2,
        0, 2, 3,
    };
    u32 ebo = mfgl_element_buffer_create(indices, MF_ArrayLength(indices));
#endif
#if 0
    float *vertices = NULL;
    u32 vbo = mfgl_vertex_buffer_dynamic_create(NULL, 512, &vertices);
    assert(vertices != NULL);
    vertices[0] = 0.0f;
    vertices[1] = 0.0f;
    vertices[2] = 0.0f;
    vertices[3] = 0.0f;

    vertices[4] = 100.0f;
    vertices[5] = 0.0f;
    vertices[6] = 1.0f;
    vertices[7] = 0.0f;

    vertices[8] = 100.0f;
    vertices[9] = 100.0f;
    vertices[10] = 1.0f;
    vertices[11] = 1.0f;

    vertices[12] = 0.0f;
    vertices[13] = 100.0f;
    vertices[14] = 0.0f;
    vertices[15] = 1.0f;
#endif

#if 0
    unsigned int *indices = NULL;
    u32 ebo = mfgl_element_buffer_dynamic_create(NULL, 512, &indices);
    assert(indices != NULL);
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;

    indices[3] = 0;
    indices[4] = 2;
    indices[5] = 3;
#endif
#if 1
    u32 ebo = 0;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    u32 indices[] = {
        0, 1, 2,
        0, 2, 3,
    };
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_DYNAMIC_DRAW);
#endif

    mfgl_vertex_buffer_bind(vbo);
    mfgl_element_buffer_bind(ebo);
    mfgl_vertex_attrib_link(0, 2, 0, 4);
    mfgl_vertex_attrib_link(1, 2, 2, 4);

    mfgl_texture_bind(characters['d'].texture_id);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

    float xmin = characters['d'].bearing.x;
    float xmax = xmin + characters['d'].size.x;
    float ymin = characters['d'].size.y - characters['d'].size.y;
    float ymax = ymin + characters['d'].size.y;

    *mf_vec_add(vertices) = xmin;
    *mf_vec_add(vertices) = ymin;
    *mf_vec_add(vertices) = 0.0f;
    *mf_vec_add(vertices) = 1.0f;

    //*mf_vec_add(vertices) = 100.0f;
    *mf_vec_add(vertices) = xmax;
    *mf_vec_add(vertices) = ymin;
    *mf_vec_add(vertices) = 1.0f;
    *mf_vec_add(vertices) = 1.0f;

    *mf_vec_add(vertices) = xmax;
    *mf_vec_add(vertices) = ymax;
    *mf_vec_add(vertices) = 1.0f;
    *mf_vec_add(vertices) = 0.0f;

    *mf_vec_add(vertices) = xmin;
    *mf_vec_add(vertices) = ymax;
    *mf_vec_add(vertices) = 0.0f;
    *mf_vec_add(vertices) = 0.0f;

    mf_vec_for(vertices) {
    //for (int i = 0; i < 16; ++i) {
        printf("%f\n", *it);
    }


    printf("vec size %d\n", mf_vec_size(vertices));

    bool running = true;
    while (running && platform.window.isOpen)
    {
        mfp_begin(&platform);
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        mfgl_vertex_buffer_bind(vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, mf_vec_size(vertices) * sizeof(float), vertices);
        //glBindBuffer(GL_ARRAY_BUFFER, 0);
        mfgl_element_buffer_bind(ebo);
        mfgl_element_buffer_draw(ebo, 6); 


        mfp_end(&platform);
        mfgl_error_check();
    }

    mfp_window_close(&platform);
    mfp_destroy(&platform);
}
