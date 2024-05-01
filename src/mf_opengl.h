#ifndef MF_OPENGL_H
#define MF_OPENGL_H
#include <stdio.h>
#include <stdbool.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <GL/gl.h>


#define API static inline
typedef GLuint MFGL_ShaderId;
typedef GLuint MFGL_ProgramId;
typedef GLuint MFGL_TextureId;


// glEnable/glDisable wrappers
void mfgl_texture(bool value);
void mfgl_blend(bool value);
API void MFGL_SetWireframe(bool value);


// Shaders
API MFGL_ShaderId MFGL_ShaderCreate(const char *vs, GLenum type);
API void MFGL_ShaderDelete(MFGL_ShaderId id);
API MFGL_ProgramId MFGL_ProgramCreate(MFGL_ShaderId vs, MFGL_ShaderId fs);
API MFGL_ProgramId MFGL_ProgramCreateVsFs(const char *vs, const char *fs);
API void MFGL_ProgramUse(MFGL_ProgramId program);
API unsigned int MFGL_ProgramUniformLocation(MFGL_ProgramId program, const char *name);
API void MFGL_ProgramSetUniform1i(unsigned int location, int a);
API void MFGL_ProgramSetUniform4f(unsigned int location, float a, float b, float c, float d);
API void MFGL_ProgramSetUniform4fv(unsigned int location, unsigned int count, bool transpose, float *data);

// Textures
API MFGL_TextureId MFGL_TextureCreate(int width, int height, GLenum targetFormat, GLenum sourceFormat, unsigned char *data);
API void MFGL_TextureBind(MFGL_TextureId id);

// Error
API void MFGL_ErrorCheck();

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





#if defined(MF_OPENGL_IMPLEMENTATION) || defined(MF_IMPLEMENTATION)

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

API void MFGL_SetWireframe(bool value) {
    if (value) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

API MFGL_ShaderId MFGL_ShaderCreate(const char *vs, GLenum type) {
    MFGL_ShaderId res;
    res = glCreateShader(type);
    glShaderSource(res, 1, &vs, NULL);
    glCompileShader(res);

    int success = 0;
    char info[512];
    glGetShaderiv(res, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(res, 512, NULL, info);
        const char *shaderName = "UNKNOWN";
        switch (type) {
            case GL_FRAGMENT_SHADER:
                shaderName = "Fragment";
                break;
            case GL_VERTEX_SHADER:
                shaderName = "Vertex";
                break;
        }
        fprintf(stderr, "ERROR compiling %s shader:\n%s", shaderName, info);
        exit(1);
    }
    return res;
}

void MFGL_ShaderDelete(unsigned int id) {
    glDeleteShader(id);
}

API MFGL_ProgramId MFGL_ProgramCreate(MFGL_ShaderId vs, MFGL_ShaderId fs) {
    MFGL_ProgramId res;
    res = glCreateProgram();
    glAttachShader(res, vs);
    glAttachShader(res, fs);
    glLinkProgram(res);

    int success = 0;
    char info[512];
    glGetProgramiv(res, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(res, 512, NULL, info);
        fprintf(stderr, "ERROR creating shader program:\n%s", info);
        exit(1);
    }
    return res;
}

API MFGL_ProgramId MFGL_ProgramCreateVsFs(const char *vs, const char *fs) {
    MFGL_ShaderId shaderVs = MFGL_ShaderCreate(vs, GL_VERTEX_SHADER);
    MFGL_ShaderId shaderFs = MFGL_ShaderCreate(fs, GL_FRAGMENT_SHADER);
    MFGL_ProgramId res = MFGL_ProgramCreate(shaderVs, shaderFs);
    MFGL_ShaderDelete(shaderVs);
    MFGL_ShaderDelete(shaderFs);
    return res;
}

void MFGL_ProgramUse(unsigned int program) {
    glUseProgram(program);
}


API unsigned int MFGL_ProgramUniformLocation(MFGL_ProgramId program, const char *name) {
    return glGetUniformLocation(program, name);
}

API void MFGL_ProgramSetUniform1i(unsigned int location, int a) {
    glUniform1i(location, a);
}

API void MFGL_ProgramSetUniform4f(unsigned int location, float a, float b, float c, float d) {
    glUniform4f(location, a, b, c, d);
}

API void MFGL_ProgramSetUniform4fv(unsigned int location, unsigned int count, bool transpose, float *data) {
    glUniformMatrix4fv(location, count, transpose, data);
}

API void MFGL_ErrorCheck() {
    GLenum code;
    code = glGetError();
    if (code != GL_NO_ERROR) {
        switch (code) {
            case 1282:
                fprintf(stderr, "Opengl invalid operation error: %d\n", code);
                break;
            default:
                fprintf(stderr, "Opengl error: %d\n", code);

        }
        exit(1);
    }
}


void mfgl_vertex_attrib_link(unsigned int location, int size, int start, int stride)
{
    glVertexAttribPointer(location, size, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void *) (start * sizeof(float)));
    MFGL_ErrorCheck();
    glEnableVertexAttribArray(location);
}

void mfgl_vertex_buffer_draw(unsigned int vbo, int n)
{
    mfgl_vertex_buffer_bind(vbo);
    glDrawArrays(GL_TRIANGLES, 0, n);
}

API MFGL_TextureId MFGL_TextureCreate(int width, int height, GLenum targetFormat, GLenum sourceFormat, unsigned char *data) {
    unsigned int res;
    glGenTextures(1, &res);
    glBindTexture(GL_TEXTURE_2D, res);
    glTexImage2D(GL_TEXTURE_2D, 0, targetFormat, width, height, 0, sourceFormat, GL_UNSIGNED_BYTE, data);
    return res;
}

API void MFGL_TextureBind(MFGL_TextureId id) {
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


#endif

#endif // MF_OPENGL_H
