#ifndef MF_OPENGL_H
#define MF_OPENGL_H
#include <stdio.h>
#include <math.h>

// NOTE: opengl code should be included by platform

typedef struct mfgl_shaders mfgl_shaderes;

// glEnable/glDisable wrappers
void mfgl_texture(bool value);
void mfgl_blend(bool value);
void mfgl_wireframe(bool value);

// Color
void mfgl_set_color(float r, float g, float b, float a);
void mfgl_set_color_i32(i32 value);
void mfgl_set_color_255(i32 r, i32 g, i32 b, i32 a);
void mfgl_set_color_rgb(float r, float g, float b);
void mfgl_set_color_rgb_255(i32 r, i32 g, i32 b);

// Viewport
void mfgl_viewport_bottom_up(u32 width, u32 height);
void mfgl_viewport_top_down(u32 width, u32 height);

// Drawing
void mfgl_clear();
void mfgl_draw_rect(float x, float y, float w, float h);
void mfgl_draw_rect_with_texture_coords(float x, float y, float w, float h, float s, float t, float sw, float th);
void mfgl_draw_circle(float x, float y, float radius);
void mfgl_draw_triangle(float a, float b, float c, float d, float e, float f);

// Shaders
void mfgl_shaders_init(mfgl_shaders *shaders, const char *vs, const char *fs);


// Textures
unsigned int mfgl_create_texture_argb(size_t width, size_t height, u32 *data);
unsigned int mfgl_create_texture_alpha(size_t width, size_t height, unsigned char *data);
void mfgl_bind_texture(unsigned int id);

// Vertex Buffer
unsigned int mfgl_vertex_buffer_create();
void mfgl_vertex_buffer_bind(unsigned int vbo);
void mfgl_vertex_attrib_link(unsigned int location, size_t n);
void mfgl_vertex_buffer_draw(unsigned int vbo, size_t n);

// Element Buffer
unsigned int mfgl_element_buffer_create(unsigned int *indices, size_t n);
void mfgl_element_buffer_bind(unsigned int ebo);
void mfgl_element_buffer_draw(unsigned int ebo, size_t n);

// Vertex Array
unsigned int mfgl_vertex_array_create();
void mfgl_vertex_array_bind(unsigned int vao);



// Error
void mfgl_error_check();


#ifdef MF_OPENGL_IMPLEMENTATION

struct mfgl_shaders
{
    bool success;
    u32 vs;
    u32 fs;
    u32 program;
};

void mfgl_texture(bool value)
{
    if (value)
        glEnable(GL_TEXTURE_2D);
    else
        glDisable(GL_TEXTURE_2D);
}

void mfgl_blend(bool value)
{
    if (value)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } else {
        glDisable(GL_BLEND);
    }
}

void mfgl_wireframe(bool value)
{
    if (value)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

}


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
        glVertex2f(x + radius * (cos((i * 2.0f * M_PI) / triangles)),
                   y + radius * (sin((i * 2.0f * M_PI) / triangles)));
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


void mfgl_shaders_init(mfgl_shaders *shaders, const char *vs, const char *fs)
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

    glGetProgramiv(shaders->program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shaders->program, 512, NULL, info);
        printf("%s", info);
        shaders->program = 0;
        return;
    }

    shaders->success = success;
    // TODO: just delete shader and use program
    // glDeleteShader(
}

void mfgl_vertex_attrib_link(unsigned int location, size_t n)
{
    glVertexAttribPointer(location, n, GL_FLOAT, GL_FALSE, n * sizeof(float), 0);
    glEnableVertexAttribArray(location);
}

void mfgl_vertex_buffer_draw(unsigned int vbo, size_t n)
{
    glDrawArrays(GL_TRIANGLES, 0, n);
}

unsigned int mfgl_create_texture_argb(size_t width, size_t height, u32 *data)
{
    unsigned int id;
    glGenTextures(1, &id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
                 width, height, 0, GL_BGRA,
                 GL_UNSIGNED_BYTE, data);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glBindTexture(GL_TEXTURE_2D, 0);
    return id;
}

unsigned int mfgl_create_texture_alpha(size_t width, size_t height, unsigned char *data)
{
    unsigned int id;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA,
                 width, height, 0, GL_ALPHA,
                 GL_UNSIGNED_BYTE, data);
    //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glBindTexture(GL_TEXTURE_2D, 0);
    return id;
}

void mfgl_bind_texture(unsigned int id)
{
    glBindTexture(GL_TEXTURE_2D, id);
}

unsigned int mfgl_vertex_buffer_create(float *vertices, size_t n)
{
    unsigned int res;
    glGenBuffers(1, &res);
    glBindBuffer(GL_ARRAY_BUFFER, res);
    glBufferData(GL_ARRAY_BUFFER, n * sizeof(float), vertices, GL_STATIC_DRAW);
    return res;
}

void mfgl_vertex_buffer_bind(unsigned int vbo)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
}

unsigned int mfgl_element_buffer_create(unsigned int *indices, size_t n)
{
	unsigned int res;
	glGenBuffers(1, &res);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, res);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, n * sizeof(unsigned int), indices, GL_STATIC_DRAW);
	return res;
}

void mfgl_element_buffer_bind(unsigned int ebo)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
}

void mfgl_element_buffer_draw(unsigned int ebo, size_t n)
{
    glDrawElements(GL_TRIANGLES, n, GL_UNSIGNED_INT, 0);
}

unsigned int mfgl_vertex_array_create()
{
    unsigned int res;
    glGenVertexArrays(1, &res);
    glBindVertexArray(res);
    return res;
}

void mfgl_vertex_array_bind(unsigned int vao)
{
    glBindVertexArray(vao);
}

void mfgl_error_check()
{
    GLenum code;
    code = glGetError();
    if (code != GL_NO_ERROR)
    {
        switch (code)
        {
            case 1282:
                fprintf(stderr, "Opengl invalid operation error: %d\n", code);
                break;
            default:
                fprintf(stderr, "Opengl error: %d\n", code);

        }
        exit(1);
    }
}


#endif

#endif // MF_OPENGL_H
