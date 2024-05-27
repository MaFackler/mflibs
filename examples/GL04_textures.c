#include "mf_lib.h"
MF_LIB_LINUX("GL")
MF_LIB_LINUX("X11")
MF_LIB_LINUX("m")
MF_LIB_WINDOWS("opengl32")
MF_LIB_WINDOWS("gdi32")
#define MF_IMPLEMENTATION

#include <mf.h>
#include <mf_file.h>
#include <mf_platform.h>
#include <mf_platform_opengl.h>
#include <mf_opengl.h>
#include <mf_image.h>


const char *VS = R"(
#version 330 core
layout (location = 0) in vec3 bPos;
layout (location = 1) in vec3 bColor;
layout (location = 2) in vec2 bTexCoord;

out vec2 texCoord;
out vec3 color;
void main() {
    gl_Position = vec4(bPos, 1.0f);
    texCoord = bTexCoord;
    color = bColor;
}
)";

const char *FS = R"(
#version 330 core
out vec4 FragColor;
in vec2 texCoord;
in vec3 color;

uniform sampler2D texture0;
uniform sampler2D texture1;

void main() {
    FragColor = mix(texture(texture0, texCoord), texture(texture1, texCoord), 0.3);
}
)";



int main(int argc, char **argv) {
    MFP_Platform platform = {0};
    MFP_Init(&platform);
    MFP_InitOpengl(&platform);

    MFP_WindowOpen(&platform,
                   "Texture",
                   MFP_WINDOWPOS_CENTER, MFP_WINDOWPOS_CENTER,
                   800, 600);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    const char *wood = "./assets/wood.jpg";
    if (!MFF_IsFile(wood)) {
        MF_Error("Could not read file '%s'\n", wood);
    }
    MFI_Image imageWood = MFI_ImageCreate(wood);
    u32 texture0 =  MFGL_TextureCreate(imageWood.width, imageWood.height,
                                       GL_RGB, GL_RGB, imageWood.data);
    glGenerateMipmap(GL_TEXTURE_2D);
    MFI_ImageFree(&imageWood);

    const char *grass = "./assets/grass.jpg";
    if (!MFF_IsFile(grass)) {
        MF_Error("Could not read file '%s'\n", grass);
    }
    MFI_Image imageGrass = MFI_ImageCreate(grass);
    u32 texture1 =  MFGL_TextureCreate(imageGrass.width, imageGrass.height,
                                       GL_RGB, GL_RGB, imageGrass.data);
    glGenerateMipmap(GL_TEXTURE_2D);
    MFI_ImageFree(&imageGrass);


    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    };

    MFGL_ProgramId program = MFGL_ProgramCreateVsFs(VS, FS);

    u32 vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    u32 vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    MFGL_ProgramUse(program);
    u32 locationTexture0 = glGetUniformLocation(program, "texture0");
    u32 locationTexture1 = glGetUniformLocation(program, "texture1");
    glUniform1i(locationTexture0, 0);
    glUniform1i(locationTexture1, 1);


    while (!MFP_WindowShouldClose(&platform)) {
        MFP_Begin(&platform);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture1);

        MFGL_ProgramUse(program);
        glBindVertexArray(vao);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        MFP_End(&platform, true);
    }

}
