#pragma comment(lib, "GL")
#pragma comment(lib, "X11")
#define MF_IMPLEMENTATION
#include <mf_platform.h>
#include <mf_platform_opengl.h>
#include <mf_opengl.h>

#include <time.h>

const char *VS = R"(
#version 330 core
layout (location = 0) in vec3 pos;

void main() {
    gl_Position = vec4(pos.x, pos.y, pos.z, 1.0f);
})";

const char *FS = R"(
#version 330 core
out vec4 FragColor;

void main() {
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
})";

int main(int argc, char **argv) {
    MFP_Platform platform = {0};
    MFP_SetTargetFps(&platform, 60);
    MFP_Init(&platform);
    MFP_InitOpengl(&platform);

    MFP_WindowOpen(&platform, "Window",
                   MFP_WINDOWPOS_CENTER, MFP_WINDOWPOS_CENTER,
                   800, 600);

    float vertices[] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3,
    };

    MFGL_ProgramId program = MFGL_ProgramCreateVsFs(VS, FS);

    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    bool wireframe = false;

    while (!MFP_WindowShouldClose(&platform)) {
        MFP_Begin(&platform);
        MFGL_ProgramUse(program);
        glBindVertexArray(vao);

        if (MFP_IsKeyPressed(&platform, 'w')) {
            wireframe = !wireframe;
            MFGL_SetWireframe(wireframe);
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);

        MFP_End(&platform, true);
    }
}
