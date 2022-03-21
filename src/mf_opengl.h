#ifndef MF_OPENGL_H
#define MF_OPENGL_H
#include <stdio.h>

// NOTE: opengl code should be included by platform

typedef struct mfgl_shaders mfgl_shaderes;

void mfgl_set_color(float r, float g, float b, float a);
void mfgl_set_color_i32(i32 value);
void mfgl_set_color_255(i32 r, i32 g, i32 b, i32 a);
void mfgl_set_color_rgb(float r, float g, float b);
void mfgl_set_color_rgb_255(i32 r, i32 g, i32 b);

void mfgl_viewport_bottom_up(u32 width, u32 height);
void mfgl_viewport_top_down(u32 width, u32 height);

void mfgl_clear();
void mfgl_draw_rect(float x, float y, float w, float h);
void mfgl_draw_triangle(float a, float b, float c, float d, float e, float f);

void mfgl_shaders_init(mfgl_shaders *shaders, char *vs, char *fs);


#ifdef MF_OPENGL_IMPLEMENTATION

struct mfgl_shaders
{
    bool success;
    u32 vs;
    u32 fs;
    u32 program;
};

void mfgl_set_color(float r, float g, float b, float a)
{
    glColor4f(r, g, b, a);
}

void mfgl_set_color_i32(i32 value)
{
    float r = (value >> 16 & 0xFF) / 255.0f;
    float g = (value >> 8 & 0xFF) / 255.0f;
    float b = (value >> 0 & 0xFF) / 255.0f;
    mfgl_set_color_rgb(r, g, b);
}

void mfgl_set_color_255(i32 r, i32 g, i32 b, i32 a)
{
    mfgl_set_color(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
}

void mfgl_set_color_rgb(float r, float g, float b)
{
    mfgl_set_color(r, g, b, 1.0f);
}

void mfgl_set_color_rgb_255(i32 r, i32 g, i32 b)
{
    mfgl_set_color_rgb(r / 255.f, g / 255.f, b / 255.f);
}

void mfgl_viewport_bottom_up(u32 width, u32 height)
{
    glMatrixMode(GL_PROJECTION);
    float mat[] =
    {
        2.0f/width, 0, 0, 0,
        0, 2.0f/height, 0, 0,
        0, 0, 1, 0,
        -1, -1, 0, 1
    };
    glLoadMatrixf(mat);

}

void mfgl_viewport_top_down(u32 width, u32 height)
{
    glMatrixMode(GL_PROJECTION);
    float mat[] =
    {
        2.0f/width, 0, 0, 0,
        0, -2.0f/height, 0, 0,
        0, 0, 1, 0,
        -1, 1, 0, 1,
    };
    glLoadMatrixf(mat);
}

void mfgl_clear()
{
    float color[4];
    glGetFloatv(GL_CURRENT_COLOR, color);
    glClearColor(color[0], color[1], color[2], color[3]);
    glClear(GL_COLOR_BUFFER_BIT);
}

void mfgl_draw_rect(float x, float y, float w, float h)
{
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(x, y);
        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(x + w, y);
        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(x + w, y + h);
        glTexCoord2f(0.0f, 1.0f);
        glVertex2f(x, y + h);
    glEnd();
}

void mfgl_draw_triangle(float a, float b, float c, float d, float e, float f)
{
    glBegin(GL_TRIANGLES);
        glVertex2f(a, b);
        glVertex2f(c, d);
        glVertex2f(e, f);
    glEnd();
}


void mfgl_shaders_init(mfgl_shaders *shaders, char *vs, char *fs)
{
    shaders->vs = 0;
    shaders->fs = 0;
    shaders->program = 0;
    int success;
    char info[512];

    // Vertex shader
    shaders->vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(shaders->vs, 1, &vs, NULL);
    glCompileShader(shaders->vs);

    glGetShaderiv(shaders->vs, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shaders->vs, 512, NULL, info);
        printf("%s", info);
        shaders->vs = 0;
        return;
    }

    // Fragment shader
    shaders->fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(shaders->fs, 1, &fs, NULL);
    glCompileShader(shaders->fs);
    glGetShaderiv(shaders->fs, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shaders->fs, 512, NULL, info);
        printf("%s", info);
        shaders->fs = 0;
        return;
    }

    // Program
    shaders->program = glCreateProgram();
    glAttachShader(shaders->program, shaders->vs);
    glAttachShader(shaders->program, shaders->fs);
    glLinkProgram(shaders->program);

    glGetShaderiv(shaders->program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shaders->program, 512, NULL, info);
        printf("%s", info);
        shaders->program = 0;
        return;
    }

    shaders->success = success;
}


#endif

#endif // MF_OPENGL_H
