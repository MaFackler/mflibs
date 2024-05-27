#include "mf_lib.h"
MF_LIB_LINUX("GL")
MF_LIB_LINUX("X11")
MF_LIB_LINUX("m")
MF_LIB_WINDOWS("opengl32")
MF_LIB_WINDOWS("gdi32")
#define MF_IMPLEMENTATION
#include <mf_platform.h>
#include <mf_platform_opengl.h>
#include <mf_opengl.h>

const char *VS = R"(
#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;
out vec3 ourColor;
void main() {
    gl_Position = vec4(pos, 1.0f);
    ourColor = color;
}
)";

const char *FS = R"(
#version 330 core
in vec3 ourColor;
out vec4 FragColor;

void main() {
    FragColor = vec4(ourColor, 1.0);
}
)";

int main(int argc, char **argv) {
    MFP_Platform platform = {0};
    MFP_SetTargetFps(&platform, 60);
    MFP_Init(&platform);
    MFP_InitOpengl(&platform);
    MFP_WindowOpen(&platform, "Shaders",
                   MFP_WINDOWPOS_CENTER, MFP_WINDOWPOS_CENTER,
                   800, 600);

    MFGL_ShaderId program = MFGL_ProgramCreateVsFs(VS, FS);
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
    };
    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    while (!MFP_WindowShouldClose(&platform)) {
        MFP_Begin(&platform);

        glUseProgram(program);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        MFP_End(&platform, true);
    }
}
