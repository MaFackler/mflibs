#define MF_IMPLEMENTATION
#define MF_SHORT_NAMES
#include "mf_lib.h"
#include "mf.h"
#include "mf_platform.h"
#include "mf_platform_opengl.h"
#include "mf_opengl.h"
#define MF_MATH_SHORT_NAMES
#include "mf_math.h"

MF_LIB_LINUX("GL")
MF_LIB_LINUX("X11")
MF_LIB_LINUX("m")
MF_LIB_WINDOWS("opengl32")
MF_LIB_WINDOWS("gdi32")

const char *VS_SRC = R"(
#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec4 aColor;
out vec4 fColor;
uniform mat4 model;
uniform mat4 view;
uniform mat4 perspective;

void main() {
    fColor = aColor;
    gl_Position = perspective * view * model * vec4(aPosition, 1.0f);
})";

const char *FS_SRC = R"(
#version 330 core
out vec4 FragColor;

in vec4 fColor;

void main() {
    // FragColor = fColor;
    FragColor = vec4(0.0f, 1.0, 0.0f, 1.0f);
})";


typedef struct Vertex {
    Vec3 pos;
    Vec4 color;
} Vertex;


int main() {
    MFP_Platform p = {};

    const int width = 800;
    const int height = 800;
    const char *title = "Craft";
    MFP_Init(&p);
    MFP_InitOpengl(&p);
    MFP_WindowOpen(&p, title, 300, 50, width, height);

    float z = -5.0f;
    float y = 1.0f;
    Vertex data[] = {
        (Vertex) {(Vec3) {-0.5f, -y, z}, (Vec4) {0.9f, 0.8f, 0.2f, 1.0f}},
        (Vertex) {(Vec3) {-0.5f, y, z}, (Vec4) {0.2f, 0.9f, 0.8f, 1.0f}},
        (Vertex) {(Vec3) {0.5f, -y, z}, (Vec4) {0.9f, 0.2f, 0.8f, 1.0f}},
        // (Vertex) {(v3) {0.5f, 0.5f, z}, (v4) {0.9f, 0.2f, 0.8f, 1.0f}},
    };
#if 0
    u32 vao = mfgl_vertex_array_create();
    mfgl_vertex_array_bind(vao);
    u32 vb = mfgl_vertex_buffer_create((float *) &data[0], 3 * 4 * 3);
    mfgl_vertex_buffer_bind(vb);
    mfgl_vertex_attrib_link(0, 3, 0, 7);
    mfgl_vertex_attrib_link(1, 4, 3, 7);
    mfgl_vertex_buffer_bin0);

    u32 indices[] = {0, 1, 2, 1, 3, 2};
    u32 ib = mfgl_element_buffer_create(&indices[0], MF_ArrayLength(indices));

#endif
    MFGL_ProgramId program = MFGL_ProgramCreateVsFs(VS_SRC, FS_SRC);
    MFGL_ProgramUse(program);

    Mat4 identity = MFM_Mat4Identity();
    // debug_sendbegin("loop");
    u32 locationModel = MFGL_ProgramUniformLocation(program, "model");
    u32 locationView = MFGL_ProgramUniformLocation(program, "view");
    u32 locationPerspective = MFGL_ProgramUniformLocation(program, "perspective");
    

    MFGL_ProgramSetUniform4fv(locationModel, 1, true, &identity.m[0]);

#if 1
    // Mat4 perspective = MFM_Mat4Ortho(-1.0f, 1.0f,
    //                                  -1.0f, 1.0f,
    //                                  0.01, 100.0f);
    Mat4 perspective = MFM_Mat4PerspectiveFov(90, width/height, 0.01, 100.0f);
    MFGL_ProgramSetUniform4fv(locationPerspective, 1, true, (float *) &perspective.m[0]);
    Vec3 eye = {0, 0, 0};
    Vec3 at = {0, 0, -10};
    Vec3 up = {0, 1, 0};
    Mat4 camera = MFM_Mat4LookAt(eye, at, up);
    MFGL_ProgramSetUniform4fv(locationView, 1, true, (float *) &camera.m);
#else
    HMM_Vec3 eye = {0.0f, 0.0f, 0.0f};
    HMM_Vec3 at = {0.0f, 0.0f, -100.0f};
    HMM_Vec3 up = {0.0f, 1.0f, 0.0f};
    HMM_Mat4 ident = HMM_M4D(1.0f);
    HMM_Mat4 camera = HMM_LookAt_RH(eye, at, up);
    mfgl_shader_uniform_4fv(locationView, 1, false, (float *) &camera.Elements[0][0]);
    // HMM_Mat4 perspective = HMM_Perspective_RH_NO(90, width/height, 1.0f, 10.0f);
    HMM_Mat4 perspective = HMM_Orthographic_RH_NO(-1.0f, 1.0f,
                                            -1.0f, 1.0f,
                                            0.01, 100.0f);
    mfgl_shader_uniform_4fv(locationPerspective, 1, false, (float *) &perspective.Elements[0]);
#endif


    MFGL_ErrorCheck();

    bool running = true;
    while (running && p.window.isOpen) {
        MFP_Begin(&p);

        if (p.input.keys[MF_KEY_ESCAPE].pressed) {
            running = false;
        }

        if (p.input.keys['1'].pressed) {
            MFP_WindowToggleFullscreen(&p);
        }

        float cameraSpeed = 0.01f;
#if 1
        if (p.input.keys[MF_KEY_LEFT].down) {
            eye.x -= cameraSpeed;
        } 
        if (p.input.keys[MF_KEY_RIGHT].down) {
            eye.x += cameraSpeed;
        }
        if (p.input.keys[MF_KEY_UP].down) {
            eye.y += cameraSpeed;
        } 
        if (p.input.keys[MF_KEY_DOWN].down) {
            eye.y -= cameraSpeed;
        }
        if (p.input.keys['w'].down) {
            eye.z -= cameraSpeed;
            printf("Camera z %f\n", eye.z);
        }
        if (p.input.keys['s'].down) {
            eye.z += cameraSpeed;
            printf("Camera z %f\n", eye.z);
        }
#endif



        glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        camera = MFM_Mat4LookAt(eye, at, up);
        MFGL_ProgramSetUniform4fv(locationView, 1, true, (float *) &camera.m[0]);

        glBegin(GL_TRIANGLES);
        for (int i = 0; i < MF_ArrayLength(data); ++i) {
            Vertex *vertex = &data[i];
            glVertex3f(vertex->pos.x, vertex->pos.y, vertex->pos.z);
            glColor3f(1.0f, 0.0f, 0.0f);
        }
        glEnd();

#if 0
        mfgl_shader_program_use(program);
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, MF_ArrayLength(indices), GL_UNSIGNED_INT, 0);
#endif

        MFP_End(&p, true);
    }

    MFP_WindowClose(&p);
    MFP_Destroy(&p);
}
