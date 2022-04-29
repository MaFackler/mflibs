#ifndef MF_GUI_H
#define MF_GUI_H

typedef struct {
    float x, y;
} mfg_vertex;

typedef struct {
    float x, y, w, h;
} mfg_rect;

#define _MF_GUI_NUM_VERTICES 512

typedef struct {
    size_t index = 0;
    mfg_vertex vertices[_MF_GUI_NUM_VERTICES];
    int width, height;

} mfg_ui;

#define _MF_GUI_ORTHO(a, b) (((a) / (2 * ((float) b))) - 1.0f)


void mfg_init(mfg_ui *ui, int width, int height);
void mfg_begin(mfg_ui *ui);
void mfg_end(mfg_ui *ui);

void mfg_button(mfg_ui *ui);

#ifdef MF_GUI_IMPLEMENTATION

void mfg_init(mfg_ui *ui, int width, int height) {
    ui->width = width;
    ui->height = height;
}

void mfg_begin(mfg_ui *ui) {
    ui->index = 0;
}

void mfg_end(mfg_ui *ui) {
}

void mfg_button(mfg_ui *ui, float x, float y) {
    mfg_rect rect = {0};
    rect.x = _MF_GUI_ORTHO(x, ui->width);
    rect.y = _MF_GUI_ORTHO(y, ui->height);
    rect.w = 200.0f / (float) ui->width;
    rect.h = 100.0f / (float) ui->height;
    if (ui->index + 5 < _MF_GUI_NUM_VERTICES) {
        ui->vertices[ui->index++] = {rect.x, rect.y};
        ui->vertices[ui->index++] = {rect.x + rect.w, rect.y};
        ui->vertices[ui->index++] = {rect.x + rect.w, rect.y + rect.h};

        ui->vertices[ui->index++] = {rect.x + rect.w, rect.y + rect.h};
        ui->vertices[ui->index++] = {rect.x, rect.y};
        ui->vertices[ui->index++] = {rect.x, rect.y + rect.h};
    }
}

#endif


#endif // MF_GUI_H
