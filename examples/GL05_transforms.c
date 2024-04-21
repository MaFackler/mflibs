#pragma comment(lib, "GL")
#pragma comment(lib, "X11")
#define MF_IMPLEMENTATION
#define MF_MATH_SHORT_NAMES
#include <mf.h>
#include <mf_math.h>
#include <mf_platform.h>
#include <mf_platform_opengl.h>
#include <mf_opengl.h>

const char *VS = R"(
#version 330 core
layout (location = 0) in vec3 bPos;
uniform mat4 transform;
void main() {
    gl_Position = transform * vec4(bPos, 1.0);
})";

const char *FS = R"(
#version 330 core
out vec4 FragColor;
void main() {
    FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
})";

int main(int argc, char **argv) {
    MFP_Platform platform = {0};
    MFP_Init(&platform);
    MFP_InitOpengl(&platform);
    MFP_SetTargetFps(&platform, 60);
    MFP_WindowOpen(&platform, "Transformations",
                   MFP_WINDOWPOS_CENTER, MFP_WINDOWPOS_CENTER,
                   800, 600);

    u32 program = MFGL_ProgramCreateVsFs(VS, FS);
    MFGL_ProgramUse(program);
    u32 locationTransform = MFGL_ProgramUniformLocation(program, "transform");

    Mat4 identity = MFM_Mat4Identity();
    // Mat4 transform = MFM_Mat4Translate((Vec3) {0.0f, 0.0f, 0.0f});
    MFGL_ProgramSetUniform4fv(locationTransform, 1, true, &identity.m[0]);

    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
    };

    u32 vbo; 
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    float speed = 0.5f;
    float scaleFactor = 1.0f;
    float angleX = 0.0f;
    float angleY = 0.0f;
    float angleZ = 0.0f;
    Vec3 pos = {0};
    float scaleTime = 0.0f;
    Mat4 transform = identity;
    Mat4 scale = identity;
    Mat4 rotationX = identity;
    Mat4 rotationY = identity;
    Mat4 rotationZ = identity;

    while (!MFP_WindowShouldClose(&platform)) {
        float dt = MFP_GetDeltaSec(&platform);
        MFP_Begin(&platform);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (MFP_IsKeyPressed(&platform, 'r')) {
            pos = (Vec3) {0};
            scaleTime = 0.0f;
            angleX = 0.0f;
            angleY = 0.0f;
            angleZ = 0.0f;
        }
        if (MFP_IsKeyDown(&platform, 't')) {
            pos.x += speed * dt;
            pos.y += speed * dt;
            if (pos.x > 1.0f) {
                pos.x = -1.0f;
                pos.y = -1.0f;
            }
        }
        if (MFP_IsKeyDown(&platform, 's')) {
            scaleTime += 0.01f;
        }
        if (MFP_IsKeyDown(&platform, 'x')) {
            angleX += 0.04f;
            angleX = MFM_Wrap(angleX, 0.0f, MFM_Pi2);
        }
        if (MFP_IsKeyDown(&platform, 'y')) {
            angleY += 0.04f;
            angleY = MFM_Wrap(angleY, 0.0f, MFM_Pi2);
        }
        if (MFP_IsKeyDown(&platform, 'z')) {
            angleZ += 0.04f;
            angleZ = MFM_Wrap(angleZ, 0.0f, MFM_Pi2);
        }

        transform = identity;
        Mat4 translate = MFM_Mat4Translate(pos);
        scaleFactor = MFM_Sin(scaleTime) + 1;
        scale = MFM_Mat4Scale(MFM_Vec3All(scaleFactor));
        rotationX = MFM_Mat4RotateX(angleX);
        rotationY = MFM_Mat4RotateY(angleY);
        rotationZ = MFM_Mat4RotateZ(angleZ);

        transform = MFM_Mat4Mul(transform, translate);
        transform = MFM_Mat4Mul(transform, scale);
        transform = MFM_Mat4Mul(transform, rotationX);
        transform = MFM_Mat4Mul(transform, rotationY);
        transform = MFM_Mat4Mul(transform, rotationZ);
        MFGL_ProgramSetUniform4fv(locationTransform, 1, true, &transform.m[0]);

        glDrawArrays(GL_TRIANGLES, 0, 3);
        MFP_End(&platform, true);
    }
}
