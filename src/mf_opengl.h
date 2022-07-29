#pragma once
#include <mf.h>

#include <stdio.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>

namespace mf { namespace gl {

// NOTE: opengl code should be included by platform


// glEnable/glDisable wrappers
void set_texture(bool value);
void set_blend(bool value);
void set_wireframe(bool value);

// Color
void set_color(f32 r, f32 g, f32 b, f32 a);
void set_color_i32(i32 value);
void set_color_255(i32 r, i32 g, i32 b, i32 a);
void set_color_rgb(f32 r, f32 g, f32 b);
void set_color_rgb_255(i32 r, i32 g, i32 b);

// Viewport
void viewport_bottom_up(u32 width, u32 height);
void viewport_top_down(u32 width, u32 height);

// Drawing
void clear();
void draw_rect(f32 x, f32 y, f32 w, f32 h);
void draw_rect_with_texture_coords(f32 x, f32 y, f32 w, f32 h, f32 s, f32 t, f32 sw, f32 th);
void draw_circle(f32 x, f32 y, f32 radius);
void draw_triangle(f32 a, f32 b, f32 c, f32 d, f32 e, f32 f);

// Shaders
u32 shader_vertex_create(const char *vs);
u32 shader_fragment_create(const char *fs);
u32 shader_program_create(u32 vs, u32 fs);
void shader_program_use(u32 program);
void shader_delete(u32 id);
u32 shader_uniform_location(u32 shader, const char *name);
void shader_uniform_4f(u32 location, f32 a, f32 b, f32 c, f32 d);
void shader_uniform_1i(u32 location, i32 a);
void shader_uniform_4fv(u32 location, u32 count, f32 *data);


// Textures
u32 texture_create_argb(i32 width, i32 height, unsigned char *data);
u32 texture_create_alpha(i32 width, i32 height, unsigned char *data);
void texture_bind(u32 id);

// Vertex Buffer
u32 vertex_buffer_create(f32 *vertices, i32 n);
u32 vertex_buffer_dynamic_create(f32 *vertices, i32 n, f32 **map);
void vertex_buffer_bind(u32 vbo);
void vertex_attrib_link(u32 location, i32 size, i32 start, i32 stride);
void vertex_buffer_draw(u32 vbo, i32 n);

// Element Buffer
u32 element_buffer_create(u32 *indices, i32 n);
u32 element_buffer_dynamic_create(u32 *indices, i32 n, u32 **map);
void element_buffer_bind(u32 ebo);
void element_buffer_draw(u32 ebo, i32 n);

// Vertex Array
u32 vertex_array_create();
void vertex_array_bind(u32 vao);



// Error
void error_check();


#ifdef MF_OPENGL_IMPLEMENTATION

void set_texture(bool value) {
    if (value)
        glEnable(GL_TEXTURE_2D);
    else
        glDisable(GL_TEXTURE_2D);
}

void set_blend(bool value) {
    if (value) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } else {
        glDisable(GL_BLEND);
    }
}

void set_wireframe(bool value) {
    if (value)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

}


void set_color(f32 r, f32 g, f32 b, f32 a) {
    glColor4f(r, g, b, a);
}

void set_color_i32(i32 value) {
    f32 r = (value >> 16 & 0xFF) / 255.0f;
    f32 g = (value >> 8 & 0xFF) / 255.0f;
    f32 b = (value >> 0 & 0xFF) / 255.0f;
    set_color_rgb(r, g, b);
}

void set_color_255(i32 r, i32 g, i32 b, i32 a) {
    set_color(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
}

void set_color_rgb(f32 r, f32 g, f32 b) {
    set_color(r, g, b, 1.0f);
}

void set_color_rgb_255(i32 r, i32 g, i32 b) {
    set_color_rgb(r / 255.f, g / 255.f, b / 255.f);
}

void viewport_bottom_up(u32 width, u32 height) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glMatrixMode(GL_COLOR);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    f32 mat[] =
    {
        2.0f/width, 0, 0, 0,
        0, 2.0f/height, 0, 0,
        0, 0, 1, 0,
        -1, -1, 0, 1
    };
    glLoadMatrixf(mat);

}

void viewport_top_down(u32 width, u32 height) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glMatrixMode(GL_COLOR);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glMatrixMode(GL_PROJECTION);
    f32 mat[] = {
        2.0f/width, 0, 0, 0,
        0, -2.0f/height, 0, 0,
        0, 0, 1, 0,
        -1, 1, 0, 1,
    };
    glLoadMatrixf(mat);
}

void clear() {
    f32 color[4];
    glGetFloatv(GL_CURRENT_COLOR, color);
    glClearColor(color[0], color[1], color[2], color[3]);
    glClear(GL_COLOR_BUFFER_BIT);
}

void draw_rect(f32 x, f32 y, f32 w, f32 h) {
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

void draw_rect_with_texture_coords(f32 x, f32 y, f32 w, f32 h, f32 s, f32 t, f32 sw, f32 th) {
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

void draw_circle(f32 x, f32 y, f32 radius) {
    glBegin(GL_TRIANGLE_FAN);
    i32 triangles = 40;
    glVertex2f(x, y);
    for (i32 i = 0; i <= triangles; ++i)
    {
        glVertex2f((f32) (x + radius * (cos((i * 2.0f * M_PI) / triangles))),
                   (f32) (y + radius * (sin((i * 2.0f * M_PI) / triangles))));
    }
    glEnd();
}

void draw_triangle(f32 a, f32 b, f32 c, f32 d, f32 e, f32 f) {
    glBegin(GL_TRIANGLES);
        glVertex2f(a, b);
        glVertex2f(c, d);
        glVertex2f(e, f);
    glEnd();
}

u32 shader_vertex_create(const char *vs) {
    u32 res;
    res = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(res, 1, &vs, NULL);
    glCompileShader(res);

    i32 success = 0;
    char info[512];
    glGetShaderiv(res, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(res, 512, NULL, info);
        printf("%s\n", info);
        exit(1);
    }
    return res;
}

u32 shader_fragment_create(const char *fs) {
    u32 res;
    // Fragment shader
    res = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(res, 1, &fs, NULL);
    glCompileShader(res);
    i32 success = 0;
    char info[512];
    glGetShaderiv(res, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(res, 512, NULL, info);
        printf("%s\n", info);
        exit(1);
    }
    return res;
}

u32 shader_program_create(u32 vs, u32 fs) {
    u32 res;
    res = glCreateProgram();
    glAttachShader(res, vs);
    glAttachShader(res, fs);
    glLinkProgram(res);

    i32 success = 0;
    char info[512];
    glGetProgramiv(res, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(res, 512, NULL, info);
        printf("%s\n", info);
        exit(1);
    }
    return res;
}

void shader_program_use(u32 program) {
    glUseProgram(program);
}

void shader_delete(u32 id) {
    glDeleteShader(id);
}

u32 shader_uniform_location(u32 shader, const char *name) {
    return glGetUniformLocation(shader, name);
}

void shader_uniform_4f(u32 location, f32 a, f32 b, f32 c, f32 d) {
    glUniform4f(location, a, b, c, d);
}

void shader_uniform_1i(u32 location, i32 a) {
    glUniform1i(location, a);
}

void shader_uniform_4fv(u32 location, u32 count, f32 *data) {
    glUniformMatrix4fv(location, count, GL_FALSE, data);
}

void vertex_attrib_link(u32 location, i32 size, i32 start, i32 stride) {
    glVertexAttribPointer(location, size, GL_FLOAT, GL_FALSE, stride * sizeof(f32), (void *) (start * sizeof(f32)));
    glEnableVertexAttribArray(location);
}

void vertex_buffer_draw(u32 vbo, i32 n) {
    vertex_buffer_bind(vbo);
    glDrawArrays(GL_TRIANGLES, 0, n);
}

u32 texture_create_argb(i32 width, i32 height, unsigned char *data) {
    u32 id;
    glGenTextures(1, &id);
    //glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, id);
    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
                 width, height, 0, GL_BGRA,
                 GL_UNSIGNED_BYTE, data);

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    return id;
}

u32 texture_create_alpha(i32 width, i32 height, unsigned char *data) {
    u32 id = 0;
    //glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA,
                 width, height, 0, GL_ALPHA,
                 GL_UNSIGNED_BYTE, data);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    return id;
}

void texture_bind(u32 id) {
    glBindTexture(GL_TEXTURE_2D, id);
}

u32 vertex_buffer_create(f32 *vertices, i32 n) {
    u32 res;
    glGenBuffers(1, &res);
    glBindBuffer(GL_ARRAY_BUFFER, res);
    glBufferData(GL_ARRAY_BUFFER, n * sizeof(f32), vertices, GL_STATIC_DRAW);
    return res;
}

u32 vertex_buffer_dynamic_create(f32 *vertices, i32 n, f32 **map) {
    u32 vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, n * sizeof(f32), vertices, GL_DYNAMIC_DRAW);
    *map = (f32 *) glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    return vbo;
}

void vertex_buffer_bind(u32 vbo) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
}

u32 element_buffer_create(u32 *indices, i32 n) {
	u32 res;
	glGenBuffers(1, &res);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, res);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, n * sizeof(u32), indices, GL_STATIC_DRAW);
	return res;
}

u32 element_buffer_dynamic_create(u32 *indices, i32 n, u32 **map) {
	u32 res;
	glGenBuffers(1, &res);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, res);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, n * sizeof(u32), indices, GL_DYNAMIC_DRAW);
    *map = (u32 *) glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
	return res;
}

void element_buffer_bind(u32 ebo) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
}

void element_buffer_draw(u32 ebo, i32 n) {
    element_buffer_bind(ebo);
    glDrawElements(GL_TRIANGLES, n, GL_UNSIGNED_INT, 0);
}

u32 vertex_array_create() {
    u32 res;
    glGenVertexArrays(1, &res);
    glBindVertexArray(res);
    return res;
}

void vertex_array_bind(u32 vao) {
    glBindVertexArray(vao);
}

void error_check() {
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
}} // mf::gl
