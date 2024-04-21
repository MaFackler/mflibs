#ifndef MF_OPENGL2_H
#define MF_OPENGL2_H
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>

// Drawing
void mfgl_clear();
void mfgl_draw_rect(float x, float y, float w, float h);
void mfgl_draw_rect_with_texture_coords(float x, float y, float w, float h, float s, float t, float sw, float th);
void mfgl_draw_circle(float x, float y, float radius);
void mfgl_draw_triangle(float a, float b, float c, float d, float e, float f);

// Color
void mfgl_set_color(float r, float g, float b, float a);
void mfgl_set_color_i32(int value);
void mfgl_set_color_255(int r, int g, int b, int a);
void mfgl_set_color_rgb(float r, float g, float b);
void mfgl_set_color_rgb_255(int r, int g, int b);

// Viewport
void mfgl_viewport_bottom_up(unsigned int width, unsigned int height);
void mfgl_viewport_top_down(unsigned int width, unsigned int height);


void mfgl_viewport_top_down(unsigned int width, unsigned int height) {
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

void mfgl_set_color(float r, float g, float b, float a)
{
    glColor4f(r, g, b, a);
}

void mfgl_set_color_i32(int value)
{
    float r = (value >> 16 & 0xFF) / 255.0f;
    float g = (value >> 8 & 0xFF) / 255.0f;
    float b = (value >> 0 & 0xFF) / 255.0f;
    mfgl_set_color_rgb(r, g, b);
}

void mfgl_set_color_255(int r, int g, int b, int a)
{
    mfgl_set_color(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
}

void mfgl_set_color_rgb(float r, float g, float b)
{
    mfgl_set_color(r, g, b, 1.0f);
}

void mfgl_set_color_rgb_255(int r, int g, int b)
{
    mfgl_set_color_rgb(r / 255.f, g / 255.f, b / 255.f);
}

void mfgl_viewport_bottom_up(unsigned int width, unsigned int height)
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

void mfgl_draw_rect_with_texture_coords(float x, float y, float w, float h, float s, float t, float sw, float th)
{
    glBegin(GL_QUADS);
        glTexCoord2f(s, t);
        glVertex2f(x, y);
        glTexCoord2f(s + sw, t);
        glVertex2f(x + w, y);
        glTexCoord2f(s + sw, t + th);
        glVertex2f(x + w, y + h);
        glTexCoord2f(s, t + th);
        glVertex2f(x, y + h);
    glEnd();
}

void mfgl_draw_circle(float x, float y, float radius)
{
    glBegin(GL_TRIANGLE_FAN);
    int triangles = 40;
    glVertex2f(x, y);
    for (int i = 0; i <= triangles; ++i)
    {
        glVertex2f((float) (x + radius * (cos((i * 2.0f * M_PI) / triangles))),
                   (float) (y + radius * (sin((i * 2.0f * M_PI) / triangles))));
    }
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

#endif // MF_OPENGL2_H
