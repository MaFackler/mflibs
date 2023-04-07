#ifndef MF_OPENGL_H
#define MF_OPENGL_H
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>

// NOTE: opengl code should be included by platform


// glEnable/glDisable wrappers
void mfgl_texture(bool value);
void mfgl_blend(bool value);
void mfgl_wireframe(bool value);




// Shaders
unsigned int mfgl_shader_vertex_create(const char *vs);
unsigned int mfgl_shader_fragment_create(const char *fs);
unsigned int mfgl_shader_program_create(unsigned int vs, unsigned int fs);
void mfgl_shader_program_use(unsigned int program);
void mfgl_shader_delete(unsigned int id);
unsigned int mfgl_shader_uniform_location(unsigned int shader, const char *name);
void mfgl_shader_uniform_4f(unsigned int location, float a, float b, float c, float d);
void mfgl_shader_uniform_1i(unsigned int location, int a);
void mfgl_shader_uniform_4fv(unsigned int location, unsigned int count, float *data);


// Textures
unsigned int mfgl_texture_create_argb(int width, int height, unsigned char *data);
unsigned int mfgl_texture_create_alpha(int width, int height, unsigned char *data);
void mfgl_texture_bind(unsigned int id);

// Vertex Buffer
unsigned int mfgl_vertex_buffer_create(float *vertices, int n);
unsigned int mfgl_vertex_buffer_dynamic_create(float *vertices, int n, float **map);
void mfgl_vertex_buffer_bind(unsigned int vbo);
void mfgl_vertex_attrib_link(unsigned int location, int size, int start, int stride);
void mfgl_vertex_buffer_draw(unsigned int vbo, int n);

// Element Buffer
unsigned int mfgl_element_buffer_create(unsigned int *indices, int n);
unsigned int mfgl_element_buffer_dynamic_create(unsigned int *indices, int n, unsigned int **map);
void mfgl_element_buffer_bind(unsigned int ebo);
void mfgl_element_buffer_draw(unsigned int ebo, int n);

// Vertex Array
unsigned int mfgl_vertex_array_create();
void mfgl_vertex_array_bind(unsigned int vao);



// Error
void mfgl_error_check();


#ifdef MF_OPENGL_IMPLEMENTATION

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









unsigned int mfgl_shader_vertex_create(const char *vs)
{
    unsigned int res;
    res = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(res, 1, &vs, NULL);
    glCompileShader(res);

    int success = 0;
    char info[512];
    glGetShaderiv(res, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(res, 512, NULL, info);
        printf("%s", info);
        exit(1);
    }
    return res;
}

unsigned int mfgl_shader_fragment_create(const char *fs)
{
    unsigned int res;
    // Fragment shader
    res = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(res, 1, &fs, NULL);
    glCompileShader(res);
    int success = 0;
    char info[512];
    glGetShaderiv(res, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(res, 512, NULL, info);
        printf("%s", info);
        exit(1);
    }
    return res;
}

unsigned int mfgl_shader_program_create(unsigned int vs, unsigned int fs)
{
    unsigned int res;
    res = glCreateProgram();
    glAttachShader(res, vs);
    glAttachShader(res, fs);
    glLinkProgram(res);

    int success = 0;
    char info[512];
    glGetProgramiv(res, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(res, 512, NULL, info);
        printf("%s", info);
        exit(1);
    }
    return res;
}

void mfgl_shader_program_use(unsigned int program)
{
    glUseProgram(program);
}

void mfgl_shader_delete(unsigned int id)
{
    glDeleteShader(id);
}

unsigned int mfgl_shader_uniform_location(unsigned int shader, const char *name)
{
    return glGetUniformLocation(shader, name);
}

void mfgl_shader_uniform_4f(unsigned int location, float a, float b, float c, float d)
{
    glUniform4f(location, a, b, c, d);
}

void mfgl_shader_uniform_1i(unsigned int location, int a)
{
    glUniform1i(location, a);
}

void mfgl_shader_uniform_4fv(unsigned int location, unsigned int count, float *data)
{
    glUniformMatrix4fv(location, count, GL_FALSE, data);
}

void mfgl_vertex_attrib_link(unsigned int location, int size, int start, int stride)
{
    glVertexAttribPointer(location, size, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void *) (start * sizeof(float)));
    mfgl_error_check();
    glEnableVertexAttribArray(location);
}

void mfgl_vertex_buffer_draw(unsigned int vbo, int n)
{
    mfgl_vertex_buffer_bind(vbo);
    glDrawArrays(GL_TRIANGLES, 0, n);
}

unsigned int mfgl_texture_create_argb(int width, int height, unsigned char *data)
{
    unsigned int id;
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    return id;
}

unsigned int mfgl_texture_create_alpha(int width, int height, unsigned char *data)
{
    unsigned int id = 0;
    //glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA,
                 width, height, 0, GL_ALPHA,
                 GL_UNSIGNED_BYTE, data);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    return id;
}

void mfgl_texture_bind(unsigned int id)
{
    glBindTexture(GL_TEXTURE_2D, id);
}

unsigned int mfgl_vertex_buffer_create(float *vertices, int n)
{
    unsigned int res;
    glGenBuffers(1, &res);
    glBindBuffer(GL_ARRAY_BUFFER, res);
    glBufferData(GL_ARRAY_BUFFER, n * sizeof(float), vertices, GL_STATIC_DRAW);
    return res;
}

unsigned int mfgl_vertex_buffer_dynamic_create(float *vertices, int n, float **map)
{
    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, n * sizeof(float), vertices, GL_DYNAMIC_DRAW);
    //*map = (float *) glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    return vbo;
}

template <typename T>
T* mfgl_map_buffer() {
    return (T*) glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
}

void mfgl_vertex_buffer_bind(unsigned int vbo)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
}

unsigned int mfgl_element_buffer_create(unsigned int *indices, int n)
{
	unsigned int res;
	glGenBuffers(1, &res);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, res);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, n * sizeof(unsigned int), indices, GL_STATIC_DRAW);
	return res;
}

unsigned int mfgl_element_buffer_dynamic_create(unsigned int *indices, int n, unsigned int **map)
{
	unsigned int res;
	glGenBuffers(1, &res);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, res);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, n * sizeof(unsigned int), indices, GL_DYNAMIC_DRAW);
    //*map = (unsigned int *) glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
	return res;
}

void mfgl_element_buffer_bind(unsigned int ebo)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
}

void mfgl_element_buffer_draw(unsigned int ebo, int n)
{
    mfgl_element_buffer_bind(ebo);
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
