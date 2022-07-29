#pragma once

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glu.h>

#define GL_COMPILE_STATUS                 0x8B81
#define GL_VERTEX_SHADER                  0x8B31
#define GL_VALIDATE_STATUS                0x8B83
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_LINK_STATUS                    0x8B82
#define WGL_CONTEXT_MAJOR_VERSION_ARB           0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB           0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB            0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB        0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#define GL_TEXTURE0                       0x84C0
#define GL_TEXTURE1                       0x84C1
#define GL_TEXTURE2                       0x84C2
#define GL_BGR                            0x80E0
#define GL_BGRA                           0x80E1
#define GL_ARRAY_BUFFER                   0x8892
#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_READ_ONLY                      0x88B8
#define GL_WRITE_ONLY                     0x88B9
#define GL_READ_WRITE                     0x88BA
#define GL_BUFFER_ACCESS                  0x88BB
#define GL_BUFFER_MAPPED                  0x88BC
#define GL_BUFFER_MAP_POINTER             0x88BD
#define GL_STREAM_DRAW                    0x88E0
#define GL_STREAM_READ                    0x88E1
#define GL_STREAM_COPY                    0x88E2
#define GL_STATIC_DRAW                    0x88E4
#define GL_STATIC_READ                    0x88E5
#define GL_STATIC_COPY                    0x88E6
#define GL_DYNAMIC_DRAW                   0x88E8
#define GL_DYNAMIC_READ                   0x88E9
#define GL_DYNAMIC_COPY                   0x88EA
#define GL_CLAMP_TO_EDGE                  0x812F
#define GL_CLAMP                          0x2900
#define GL_CLAMP_TO_BORDER                0x812D

#define GL_FUNC_DEF(rtype, name, ...) \
    typedef rtype (*name##Proc)(__VA_ARGS__); \
    static name##Proc name;

GL_FUNC_DEF(HGLRC, wglCreateContextAttribsARB, HDC hDC, HGLRC hShareContext, const int *attribList);
// TODO: why gl char not working
GL_FUNC_DEF(GLint, glGetUniformLocation, GLuint program, const char *name);
GL_FUNC_DEF(GLuint, glCreateProgram, void);
GL_FUNC_DEF(GLuint, glCreateShader, GLenum type);
GL_FUNC_DEF(void*, glMapBuffer, GLenum target, GLenum access);
GL_FUNC_DEF(void, glActiveTexture, GLenum texture);
GL_FUNC_DEF(void, glAttachShader, GLuint program, GLuint shader);
GL_FUNC_DEF(void, glBindBuffer, GLenum target, GLuint buffer);
GL_FUNC_DEF(void, glBindBuffers, GLenum target, GLuint first, GLsizei count, const GLuint *buffers);
GL_FUNC_DEF(void, glBindVertexArray, GLuint array);
GL_FUNC_DEF(void, glBufferData, GLenum target, GLsizei size, const void *data, GLenum usage);
GL_FUNC_DEF(void, glBufferSubData, GLenum target, GLint *offset, GLsizei size, const void *data);
GL_FUNC_DEF(void, glCompileShader, GLuint shader);
GL_FUNC_DEF(void, glDeleteProgram, GLuint program);
GL_FUNC_DEF(void, glDeleteVertexArrays, GLsizei n, const GLuint *arrays);
GL_FUNC_DEF(void, glDeleteShader, GLuint shader);
GL_FUNC_DEF(void, glEnableVertexAttribArray, GLuint index);
GL_FUNC_DEF(void, glGenBuffers, GLsizei n, GLuint *buffers);
GL_FUNC_DEF(void, glDeleteBuffers, GLsizei n, const GLuint *buffers);
GL_FUNC_DEF(void, glGenVertexArrays, GLsizei n, GLuint *arrays);
GL_FUNC_DEF(void, glGenerateMipmap, GLenum target);
GL_FUNC_DEF(void, glGetProgramiv, GLuint program, GLenum pname, GLint *params);
GL_FUNC_DEF(void, glGetShaderInfoLog, GLuint shader, GLsizei bufSIze, GLsizei *length, char *infoLog);
GL_FUNC_DEF(void, glGetShaderiv, GLuint shader, GLenum pname, GLint *params);
GL_FUNC_DEF(void, glLinkProgram, GLuint program);
GL_FUNC_DEF(void, glShaderSource, GLuint shader, GLsizei count, const char **string, GLint *length);
GL_FUNC_DEF(void, glUniform1i, GLint location, GLint v0);
GL_FUNC_DEF(void, glUniform4f, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
GL_FUNC_DEF(void, glUniformMatrix4fv, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
GL_FUNC_DEF(void, glUseProgram, GLuint program);
GL_FUNC_DEF(void, glVertexAttribPointer, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void * pointer);
#undef GL_FUNC_DEF

void opengl_init(mf::platform::Platform &p) {
    PIXELFORMATDESCRIPTOR pf = {};
    pf.nSize = sizeof(pf);
    pf.nVersion = 1;
    pf.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
    pf.cColorBits = 24;
    pf.cAlphaBits = 8;
    int suggestedIndex = ChoosePixelFormat(p.dc, &pf);
    PIXELFORMATDESCRIPTOR spf;
    DescribePixelFormat(p.dc, suggestedIndex, sizeof(spf), &spf);
    SetPixelFormat(p.dc, suggestedIndex, &spf);

    HGLRC basicContext = wglCreateContext(p.dc);
    if (wglMakeCurrent(p.dc, basicContext)) {
#define GL_FUNC_LOAD(name)\
    name = (name##Proc) wglGetProcAddress(#name);
        wglCreateContextAttribsARB = (wglCreateContextAttribsARBProc) wglGetProcAddress("wglCreateContextAttribsARB");
        GLint attribs[] =
        {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
            WGL_CONTEXT_MINOR_VERSION_ARB, 3,
            WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
            0
        };
        HGLRC modernContext = wglCreateContextAttribsARB(p.dc, 0, &attribs[0]);
        if (modernContext) {
            wglMakeCurrent(p.dc, modernContext);
            wglDeleteContext(basicContext);
            p.hgl = modernContext;
        }
        else {
            p.hgl = basicContext;
        }
        GL_FUNC_LOAD(glActiveTexture);
        GL_FUNC_LOAD(glAttachShader);
        GL_FUNC_LOAD(glBindBuffer);
        GL_FUNC_LOAD(glDeleteBuffers);
        GL_FUNC_LOAD(glBindBuffers);
        GL_FUNC_LOAD(glBindVertexArray);
        GL_FUNC_LOAD(glBufferData);
        GL_FUNC_LOAD(glBufferSubData);
        GL_FUNC_LOAD(glCompileShader);
        GL_FUNC_LOAD(glCreateProgram);
        GL_FUNC_LOAD(glCreateShader);
        GL_FUNC_LOAD(glDeleteShader);
        GL_FUNC_LOAD(glEnableVertexAttribArray);
        GL_FUNC_LOAD(glGenBuffers);
        GL_FUNC_LOAD(glDeleteVertexArrays);
        GL_FUNC_LOAD(glGenVertexArrays);
        GL_FUNC_LOAD(glGenerateMipmap);
        GL_FUNC_LOAD(glGetProgramiv);
        GL_FUNC_LOAD(glGetShaderInfoLog);
        GL_FUNC_LOAD(glGetShaderiv);
        GL_FUNC_LOAD(glGetUniformLocation);
        GL_FUNC_LOAD(glLinkProgram);
        GL_FUNC_LOAD(glMapBuffer);
        GL_FUNC_LOAD(glShaderSource);
        GL_FUNC_LOAD(glUniform1i);
        GL_FUNC_LOAD(glUniform4f);
        GL_FUNC_LOAD(glUniformMatrix4fv);
        GL_FUNC_LOAD(glUseProgram);
        GL_FUNC_LOAD(glDeleteProgram);
        GL_FUNC_LOAD(glVertexAttribPointer);
    }
}
