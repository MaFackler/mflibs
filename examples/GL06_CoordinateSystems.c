#pragma comment(lib, "GL")
#pragma comment(lib, "X11")
#define MF_IMPLEMENTATION
#include <mf.h>
#include <mf_platform.h>
#include <mf_platform_opengl.h>
#include <mf_opengl.h>
#include <mf_file.h>
#include <mf_image.h>
#include <mf_vertex_factory.h>

#define MF_MATH_SHORT_NAMES
#include <mf_math.h>

const char *VS = R"(
#version 330 core
layout (location = 0) in vec3 bPos;
layout (location = 1) in vec2 bTex;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
out vec2 texCoord;
void main() {
    gl_Position = projection * view * model * vec4(bPos, 1.0);
    texCoord = bTex;
})";

const char *FS = R"(
#version 330 core
out vec4 FragColor;
in vec2 texCoord;
uniform sampler2D sampler;
void main() {
    FragColor = texture(sampler, texCoord);
    // FragColor = vec4(texCoord.x, texCoord.y, 0.0f, 1.0f);
})";

int main() {
    MFP_Platform platform = {0};
    MFP_Init(&platform);
    MFP_InitOpengl(&platform);
    const int WIDTH = 800;
    const int HEIGHT = 600;
    MFP_WindowOpen(&platform, "Coordinate Systems",
                   MFP_WINDOWPOS_CENTER, MFP_WINDOWPOS_CENTER,
                   WIDTH, HEIGHT);
    float s = 0.5f;
    u32 nvertices = 0;
    u32 nindices = 0;
    MFV_VertexF3F2 *vertices = MFV_VertexF3F2CubeCreate(&nvertices);
    MFV_IndicesTriangle *indices = MFV_IndicesCubeCreate(&nindices);

    MFM_Vec3 cubePositions[] = {
        { 0.0f,  0.0f,  0.0f},
        { 2.0f,  5.0f, -15.0f},
        {-1.5f, -2.2f, -2.5f},
        {-3.8f, -2.0f, -12.3f}, 
        { 2.4f, -0.4f, -3.5f},
        {-1.7f,  3.0f, -7.5f},
        { 1.3f, -2.0f, -2.5f},
        { 1.5f,  2.0f, -2.5f},
        { 1.5f,  0.2f, -1.5f},
        {-1.3f,  1.0f, -1.5f}
    };
    MF_Assert(5 * sizeof(float) == sizeof(MFV_VertexF3F2));
    MF_Assert(nvertices == 24);
    MF_Assert(nindices == 12);

    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);
    u32 vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(MFV_VertexF3F2) * nvertices, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MFV_VertexF3F2), (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(MFV_VertexF3F2), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    u32 program = MFGL_ProgramCreateVsFs(VS, FS);
    MFGL_ProgramUse(program);

    u32 ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(MFV_IndicesTriangle) * nindices, indices, GL_STATIC_DRAW);

    Mat4 projection = MFM_Mat4PerspectiveFov(MFM_DegToRad(45), (float) WIDTH / (float) HEIGHT, 1.0f, 100.0f);
    Mat4 view = MFM_Mat4Translate(0.0f, 0.0f, -3.0f);
    u32 locationModel = MFGL_ProgramUniformLocation(program, "model");
    u32 locationView = MFGL_ProgramUniformLocation(program, "view");
    u32 locationProjection = MFGL_ProgramUniformLocation(program, "projection");
    MFGL_ProgramSetUniform4fv(locationView, 1, true, &view.m[0]);
    MFGL_ProgramSetUniform4fv(locationProjection, 1, true, &projection.m[0]);

    const char *wood = "./assets/wood.jpg";
    if (!MFF_IsFile(wood)) {
        MF_Error("Could not read file '%s'\n", wood);
    }
    MFI_Image image = MFI_ImageCreate(wood);
    u32 texture = MFGL_TextureCreate(image.width, image.height,
                                     GL_RGB, GL_RGB, image.data);
    glGenerateMipmap(GL_TEXTURE_2D);
    MFI_ImageFree(&image);

    while (!MFP_WindowShouldClose(&platform)) {
        MFP_Begin(&platform);
        double time = MFP_GetTimeSec(&platform);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        MF_ArrayFor(cubePositions) {
            Mat4 model = MFM_Mat4Translate(it->x, it->y, it->z);
            model = MFM_Mat4Mul(model, MFM_Mat4RotateAxis((Vec3) {0.5f, 1.0f, 0.0f}, time));
            MFGL_ProgramSetUniform4fv(locationModel, 1, true, &model.m[0]);
            glDrawElements(GL_TRIANGLES, sizeof(MFV_IndicesTriangle) * nindices, GL_UNSIGNED_INT, 0);
        }

        MFP_End(&platform, true);
    }
}
