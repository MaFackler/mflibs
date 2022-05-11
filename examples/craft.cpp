#define MF_PLATFORM_USE_OPENGL
#define MF_PLATFORM_IMPLEMENTATION
#include "mf_platform.h"

#define MF_OPENGL_IMPLEMENTATION
#include "mf_opengl.h"

#define MF_MATH_IMPLEMENTATION
#include "mf_math.h"

typedef mfm_v3 v3;
typedef mfm_v4 v4;
typedef unsigned int u32;

const char *VS_SRC = R"(
#version 460 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec4 aColor;
out vec4 fColor;

void main() {
    fColor = aColor;
    gl_Position = vec4(aPosition, 1.0f);
})";

const char *FS_SRC = R"(
#version 460 core
out vec4 FragColor;

in vec4 fColor;

void main() {
    FragColor = fColor;
})";


struct Vertex {
    v3 pos;
    v4 color;
};

int main() {
    mfp_platform p = {};

    const int width = 1280;
    const int height = 720;
    const char *title = "Craft";
    mfp_init(&p);
    mfp_window_open(&p, title, 0, 0, width, height);
    //glViewport(0, 0, width, height);


    Vertex data[3] = {
        {v3{-0.5f, -0.5f, 0.0f}, v4{0.9f, 0.8f, 0.2f, 1.0f}},
        {v3{0.0f, 0.5f, 0.0f}, v4{0.2f, 0.9f, 0.8f, 1.0f}},
        {v3{0.5f, -0.5f, 0.0f}, v4{0.9f, 0.2f, 0.8f, 1.0f}},
    };
    u32 vao = mfgl_vertex_array_create();
    mfgl_vertex_buffer_bind(vao);
    u32 vb = mfgl_vertex_buffer_create((float *) &data[0], 3 * 4 * 3);
    mfgl_vertex_buffer_bind(vb);
    mfgl_vertex_attrib_link(0, 3, 0, 7);
    mfgl_vertex_attrib_link(1, 4, 3, 7);
    mfgl_vertex_buffer_bind(0);

    u32 indices[] = {0, 1, 2};
    u32 ib = mfgl_element_buffer_create(&indices[0], 3);

    u32 vs = mfgl_shader_vertex_create(VS_SRC);
    u32 fs = mfgl_shader_fragment_create(FS_SRC);
    u32 program = mfgl_shader_program_create(vs, fs);
    mfgl_shader_delete(vs);
    mfgl_shader_delete(fs);

    bool running = true;
    while (running && p.window.isOpen) {
        mfp_begin(&p);

        if (p.input.keys[MF_KEY_ESCAPE].pressed) {
            running = false;
        }

        if (p.input.keys['1'].pressed) {
            mfp_window_toggle_fullscreen(&p);
        }

        if (p.input.mouseX > 50) {
            //printf("Mouse is greather that 50.\n");
        }

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        mfgl_shader_program_use(program);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        mfp_end(&p, true);
    }

    mfp_window_close(&p);
    mfp_destroy(&p);
}
