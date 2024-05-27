#include "mf_lib.h"
MF_LIB_LINUX("GL")
MF_LIB_LINUX("X11")
MF_LIB_LINUX("m")
MF_LIB_WINDOWS("opengl32")
MF_LIB_WINDOWS("gdi32")
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
uniform vec3 viewPos;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;

void main() {
    // ambient
    vec3 ambient = material.ambient * light.ambient;

    // diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = (diff * material.diffuse) * light.diffuse;

    // specular
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = (material.specular * spec) * light.specular;

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
    MFP_WindowOpen(&platform, "Materials",
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

    Vec3 lightColor = {0};
    // Vec3 diffuseColor = {0};
    Vec3 lightPos = {1.2f, 1.0f, 2.0f};
    Vec3 viewPos = (Vec3) {-1.4f, 2.0f, -5.0f};
    u32 program = MFGL_ProgramCreateVsFs(VS, FS);
    MFGL_ProgramUse(program);
    u32 locationModel = MFGL_ProgramUniformLocation(program, "model");
    u32 locationView = MFGL_ProgramUniformLocation(program, "view");
    u32 locationProjection = MFGL_ProgramUniformLocation(program, "projection");
    u32 locationObjectColor = MFGL_ProgramUniformLocation(program, "objectColor");
    u32 locationViewPos = MFGL_ProgramUniformLocation(program, "viewPos");
    u32 locationMaterialAmbient = MFGL_ProgramUniformLocation(program, "material.ambient");
    u32 locationMaterialDiffuse = MFGL_ProgramUniformLocation(program, "material.diffuse");
    u32 locationMaterialSpecular = MFGL_ProgramUniformLocation(program, "material.specular");
    u32 locationMaterialShininess = MFGL_ProgramUniformLocation(program, "material.shininess");
    u32 locationLightPosition = MFGL_ProgramUniformLocation(program, "light.position");
    u32 locationLightAmbient = MFGL_ProgramUniformLocation(program, "light.ambient");
    u32 locationLightDiffuse = MFGL_ProgramUniformLocation(program, "light.diffuse");
    u32 locationLightSpecular = MFGL_ProgramUniformLocation(program, "light.specular");
    Mat4 identity = MFM_Mat4Identity();
    Mat4 projection = MFM_Mat4PerspectiveFov(MFM_DegToRad(45), 800.0f / 600.0f, 0.1f, 100.0f);
    Mat4 view = MFM_Mat4LookAt(viewPos, (Vec3) {0.0f, 0.0f, 0.0f}, (Vec3) {0.0f, 1.0f, 0.0f});
    MFGL_ProgramSetUniform4fv(locationModel, 1, true, &identity.m[0]);
    MFGL_ProgramSetUniform4fv(locationView, 1, true, &view.m[0]);
    MFGL_ProgramSetUniform4fv(locationProjection, 1, true, &projection.m[0]);
    MFGL_ProgramSetUniform3f(locationObjectColor, 1.0f, 0.5f, 0.31f);
    MFGL_ProgramSetUniform3f(locationViewPos, viewPos.x, viewPos.y, viewPos.z);
    MFGL_ProgramSetUniform3f(locationMaterialAmbient, 1.0f, 0.5f, 0.31f);
    MFGL_ProgramSetUniform3f(locationMaterialDiffuse, 1.0f, 0.5f, 0.31f);
    MFGL_ProgramSetUniform3f(locationMaterialSpecular, 0.5f, 0.5f, 0.5);
    MFGL_ProgramSetUniform1f(locationMaterialShininess, 32.0f);
    MFGL_ProgramSetUniform3f(locationLightAmbient, 0.2f, 0.2f, 0.2f);
    MFGL_ProgramSetUniform3f(locationLightDiffuse, 0.5f, 0.5f, 0.5f);
    MFGL_ProgramSetUniform3f(locationLightSpecular, 1.0f, 1.0f, 1.0f);
    MFGL_ProgramSetUniform3f(locationLightPosition, lightPos.x, lightPos.y, lightPos.z);

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

        // Main Cube
        MFGL_ProgramUse(program);
        float delta = MFP_GetTimeSec(&platform);
        lightColor.x = MFM_Sin(delta * 2.0f);
        lightColor.y = MFM_Sin(delta * 0.7f);
        lightColor.z = MFM_Sin(delta * 1.3f);
        Vec3 diffuseColor = MFM_Vec3Mul(lightColor, (Vec3){0.5f, 0.5f, 0.5f});
        Vec3 ambientColor = MFM_Vec3Mul(lightColor, (Vec3){0.2f, 0.2f, 0.2f});
        MFGL_ProgramSetUniform3f(locationLightAmbient, ambientColor.x, ambientColor.y, ambientColor.z);
        MFGL_ProgramSetUniform3f(locationLightDiffuse, diffuseColor.x, diffuseColor.y, diffuseColor.z);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / sizeof(float));

        // Light cube
        MFGL_ProgramUse(programLight);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / sizeof(float));
        MFP_End(&platform, true);
    }
}
