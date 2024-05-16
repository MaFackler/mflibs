#pragma comment(lib, "GL")
#pragma comment(lib, "X11")
#define MF_IMPLEMENTATION
#include <mf.h>
#include <mf_platform.h>
#include <mf_platform_opengl.h>
#include <mf_opengl.h>
#define MF_MATH_SHORT_NAMES
#include <mf_math.h>

const char *VS = R"(
#version 330 core
layout (location = 0) in vec3 bPos;
layout (location = 1) in vec3 bNormal;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
out vec3 normal;
out vec3 fragPos;
void main() {
    gl_Position = projection * view * model * vec4(bPos, 1.0);
    // NOTE: this is for non uniform scaling
    normal = mat3(transpose(inverse(model))) * bNormal;
    // normal = bNormal;
    fragPos = vec3(model * vec4(bPos, 1.0f));
})";

const char *FS = R"(
#version 330 core
out vec4 FragColor;
in vec3 normal;
in vec3 fragPos;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
void main() {
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
})";

const char *FS_LIGHT = R"(
#version 330 core
out vec4 FragColor;
void main() {
    FragColor = vec4(1.0);
})";

int main(int argc, char **argv) {

    MFP_Platform platform = {0};
    MFP_SetTargetFps(&platform, 60);
    MFP_Init(&platform);
    MFP_InitOpengl(&platform);
    MFP_WindowOpen(&platform, "Basic Lighting",
                   MFP_WINDOWPOS_CENTER, MFP_WINDOWPOS_CENTER,
                   800, 600);

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };


    glEnable(GL_DEPTH_TEST);
    u32 vbo, vao;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    Vec3 lightPos = {1.2f, 1.0f, 2.0f};
    Vec3 viewPos = (Vec3) {-1.4f, 2.0f, -5.0f};
    u32 program = MFGL_ProgramCreateVsFs(VS, FS);
    MFGL_ProgramUse(program);
    u32 locationModel = MFGL_ProgramUniformLocation(program, "model");
    u32 locationView = MFGL_ProgramUniformLocation(program, "view");
    u32 locationProjection = MFGL_ProgramUniformLocation(program, "projection");
    u32 locationObjectColor = MFGL_ProgramUniformLocation(program, "objectColor");
    u32 locationLightColor = MFGL_ProgramUniformLocation(program, "lightColor");
    u32 locationLightPos = MFGL_ProgramUniformLocation(program, "lightPos");
    u32 locationViewPos = MFGL_ProgramUniformLocation(program, "viewPos");
    Mat4 identity = MFM_Mat4Identity();
    Mat4 projection = MFM_Mat4PerspectiveFov(MFM_DegToRad(45), 800.0f / 600.0f, 0.1f, 100.0f);
    Mat4 view = MFM_Mat4LookAt(viewPos, (Vec3) {0.0f, 0.0f, 0.0f}, (Vec3) {0.0f, 1.0f, 0.0f});
    MFGL_ProgramSetUniform4fv(locationModel, 1, true, &identity.m[0]);
    MFGL_ProgramSetUniform4fv(locationView, 1, true, &view.m[0]);
    MFGL_ProgramSetUniform4fv(locationProjection, 1, true, &projection.m[0]);
    MFGL_ProgramSetUniform3f(locationObjectColor, 1.0f, 0.5f, 0.31f);
    MFGL_ProgramSetUniform3f(locationLightColor, 1.0f, 1.0f, 1.0f);
    MFGL_ProgramSetUniform3f(locationLightPos, lightPos.x, lightPos.y, lightPos.z);
    MFGL_ProgramSetUniform3f(locationViewPos, viewPos.x, viewPos.y, viewPos.z);

    u32 programLight = MFGL_ProgramCreateVsFs(VS, FS_LIGHT);
    MFGL_ProgramUse(programLight);
    Mat4 model = MFM_Mat4TranslateV(lightPos);
    model = MFM_Mat4Mul(model, MFM_Mat4Scale((Vec3){0.2f, 0.2f, 0.2f}));
    u32 locationModelLight = MFGL_ProgramUniformLocation(programLight, "model");
    u32 locationViewLight = MFGL_ProgramUniformLocation(programLight, "view");
    u32 locationProjectionLight = MFGL_ProgramUniformLocation(programLight, "projection");
    MFGL_ProgramSetUniform4fv(locationModelLight, 1, true, &model.m[0]);
    MFGL_ProgramSetUniform4fv(locationViewLight, 1, true, &view.m[0]);
    MFGL_ProgramSetUniform4fv(locationProjectionLight, 1, true, &projection.m[0]);

    while (!MFP_WindowShouldClose(&platform)) {
        MFP_Begin(&platform);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        MFGL_ProgramUse(program);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / sizeof(float));

        MFGL_ProgramUse(programLight);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / sizeof(float));
        MFP_End(&platform, true);
    }
}
