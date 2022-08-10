namespace example_craft {

struct ExampleCraft : IExample {

    u32 program;
    u32 indices[6];
    u32 vao;
    u32 vb;

    void init(IPlatform &p, Renderer2D &r);
    void update(IPlatform &p);
    void render(IPlatform &p, Renderer2D &r);
    void shutdown(IPlatform &p);
};

void ExampleCraft::init(IPlatform &p, Renderer2D &r) {
    const char *VS_SRC = R"(
#version 460 core
    layout (location = 0) in vec3 aPosition;
    layout (location = 1) in vec4 aColor;
    out vec4 fColor;
    uniform mat4 transform;
    uniform mat4 perspective;
    uniform mat4 view;

    void main() {
        fColor = aColor;
        gl_Position = perspective * view * (transform * vec4(aPosition, 1.0f));
    })";

    const char *FS_SRC = R"(
#version 460 core
    out vec4 FragColor;

    in vec4 fColor;

    void main() {
        FragColor = fColor;
    })";

    float z = 10.0f;
    Vertex data[] = {
        {v3{-0.5f, -0.5f, z}, v4{0.9f, 0.8f, 0.2f, 1.0f}},
        {v3{-0.5f, 0.5f, z}, v4{0.2f, 0.9f, 0.8f, 1.0f}},
        {v3{0.5f, -0.5f, z}, v4{0.9f, 0.2f, 0.8f, 1.0f}},
        {v3{0.5f, 0.5f, z}, v4{0.9f, 0.2f, 0.8f, 1.0f}},
    };
    this->vao = vertex_array_create();
    vertex_buffer_bind(vao);
    this->vb = vertex_buffer_create((float *) &data[0], 3 * 4 * 3);
    vertex_buffer_bind(vb);
    vertex_attrib_link(0, 3, 0, 7);
    vertex_attrib_link(1, 4, 3, 7);
    vertex_buffer_bind(0);

    this->indices[0] = 0;
    this->indices[1] = 1;
    this->indices[2] = 2;
    this->indices[3] = 1;
    this->indices[4] = 3;
    this->indices[5] = 2;
    u32 ib = element_buffer_create(&indices[0], MF_ArrayLength(indices));

    u32 vs = shader_vertex_create(VS_SRC);
    u32 fs = shader_fragment_create(FS_SRC);
    this->program = shader_program_create(vs, fs);
    shader_delete(vs);
    shader_delete(fs);
    shader_program_use(this->program);

    u32 location_transform = shader_uniform_location(this->program, "transform");
    m4 transform = mf::m4_create_identity();
    shader_uniform_4fv(location_transform, 1, &transform.m[0]);

    u32 location_perspective = shader_uniform_location(this->program, "perspective");
    m4 perspective = mf::m4_create_perspective(60.0f, 16.0f/9.0f, 0.0001f, 100);
    shader_uniform_4fv(location_perspective, 1, (float *) &perspective.m);

    m4 view = mf::m4_create_look_at({0.0f, 0.0f, 20.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
    u32 location_view = shader_uniform_location(this->program, "view");
    shader_uniform_4fv(location_view, 1, (float *) view.m);
    error_check();
}

void ExampleCraft::update(IPlatform &p) {

    if (p.input.keys['1'].pressed) {
    }

    if (p.input.mouseX > 50) {
        //printf("Mouse is greather that 50.\n");
    }
}

void ExampleCraft::render(IPlatform &p, Renderer2D &r) {
    shader_program_use(this->program);
    glDrawElements(GL_TRIANGLES, MF_ArrayLength(this->indices), GL_UNSIGNED_INT, 0);
}

void ExampleCraft::shutdown(IPlatform &p) {
    glDeleteProgram(this->program);
    glDeleteBuffers(1, &this->vb);
    glDeleteVertexArrays(1, &this->vao);

}

}
