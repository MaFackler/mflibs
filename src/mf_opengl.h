#ifndef MF_OPENGL_H
#define MF_OPENGL_H
#define GL_GLEXT_PROTOTYPES
#define GLX_GLXEXT_PROTOTYPES
#include <GL/gl.h>
#ifdef MF_LINUX
#include <GL/glx.h>
#include <GL/glxext.h>
#include <GL/glext.h>
#endif


void mfgl_set_color(float r, float g, float b, float a);
void mfgl_set_color_255(i32 r, i32 g, i32 b, i32 a);
void mfgl_set_color_rgb(float r, float g, float b);
void mfgl_set_color_rgb_255(i32 r, i32 g, i32 b);

void mfgl_viewport_bottom_up(u32 width, u32 height);

void mfgl_clear();
void mfgl_draw_rect(float x, float y, float w, float h);


#ifdef MF_OPENGL_IMPLEMENTATION

void mfgl_set_color(float r, float g, float b, float a)
{
    glColor4f(r, g, b, a);
}

void mfgl_set_color_rgb_255(i32 r, i32 g, i32 b, i32 a)
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


#endif

#endif // MF_OPENGL_H
