#ifndef MF_GUI_H
#define MF_GUI_H

#include <stddef.h>

typedef struct {
    float x, y;
    float r, g, b;
    float u, t;
} mfg_vertex;

typedef struct {
    float x, y, w, h;
} mfg_rect;

#define _MF_GUI_NUM_INDICIES 512

typedef struct {
    size_t verticesIndex;
    // TODO: what is the best size
    mfg_vertex vertices[_MF_GUI_NUM_INDICIES];
    size_t indicesIndex;
    unsigned int indices[_MF_GUI_NUM_INDICIES];
    int width, height;

} mfg_ui;

#define _MF_GUI_ORTHO(a, b) (((a) / (2 * ((float) b))) - 1.0f)


void mfg_init(mfg_ui *ui, int width, int height);
void mfg_begin(mfg_ui *ui);
void mfg_end(mfg_ui *ui);

void mfg_button(mfg_ui *ui);
void mfg_text(mfg_ui *ui, const char *text);

#ifdef MF_GUI_IMPLEMENTATION

void mfg_init(mfg_ui *ui, int width, int height) {
    ui->width = width;
    ui->height = height;
}

void mfg_begin(mfg_ui *ui) {
    ui->verticesIndex = 0;
    ui->indicesIndex = 0;
}

void mfg_end(mfg_ui *ui) {
}

void mfg_button(mfg_ui *ui, float x, float y, float w, float h) {
    mfg_rect rect = {0};
    rect.x = _MF_GUI_ORTHO(x, ui->width);
    rect.y = _MF_GUI_ORTHO(y, ui->height);
    rect.w = w / (float) ui->width;
    rect.h = h / (float) ui->height;
    // TODO: check size
    ui->vertices[ui->vertices_index + 0] = {rect.x, rect.y, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f};
    ui->vertices[ui->vertices_index + 1] = {rect.x + rect.w, rect.y, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f};
    ui->vertices[ui->vertices_index + 2] = {rect.x + rect.w, rect.y + rect.h, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
    ui->vertices[ui->vertices_index + 3] = {rect.x, rect.y + rect.h, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f};

    ui->indices[ui->indicesIndex++] = ui->vertices_index + 0;
    ui->indices[ui->indicesIndex++] = ui->vertices_index + 1;
    ui->indices[ui->indicesIndex++] = ui->vertices_index + 2;

    ui->indices[ui->indicesIndex++] = ui->vertices_index + 0;
    ui->indices[ui->indicesIndex++] = ui->vertices_index + 2;
    ui->indices[ui->indicesIndex++] = ui->vertices_index + 3;

    ui->vertices_index += 4;
}

void mfg_text(mfg_ui *ui, const char *text) {
}

#endif


#endif // MF_GUI_H
