#ifndef MF_FONT_H
#define MF_FONT_H

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

typedef struct {
    unsigned char *data;
    size_t dim;
    stbtt_bakedchar chardata[96];
} mffo_font;

typedef struct {
    float x0, x1, y0, y1;
    float s0, s1, t0, t1;
} mffo_charrect;

void mffo_font_alloc(mffo_font *font, const char *filename);

#define _MF_FONT_START_CHAR 32
#define _MF_FONT_END_CHAR 128

#ifdef MF_FONT_IMPLEMENTATION

void mffo_font_alloc(mffo_font *font, const char *filename) {
    const size_t dim = 512;
    unsigned char file_contents[1<<20];
    font->data = (unsigned char *) malloc(dim * dim);
    fread(file_contents, 1, 1<<20, fopen(filename, "rb"));
    stbtt_BakeFontBitmap(file_contents,
                         0, 32.0,
                         font->data, dim, dim,
                         _MF_FONT_START_CHAR, _MF_FONT_END_CHAR - _MF_FONT_START_CHAR, font->chardata);
    font->dim = dim;
}

bool mffo_is_supported_char(mffo_font *font, char c) {
    return (c >= _MF_FONT_START_CHAR && c < _MF_FONT_START_CHAR);
}

void mffo_font_get_charrect(mffo_font *font, mffo_charrect *charrect, char c) {
    stbtt_aligned_quad q;
    float x;
    float y;
    stbtt_GetBakedQuad(font->chardata, font->dim, font->dim,
                       c - _MF_FONT_START_CHAR,
                       &x, &y, &q, 1);
    charrect->x0 = q.x0; 
    charrect->x1 = q.x1; 
    charrect->y0 = q.y0; 
    charrect->y1 = q.y1; 

    charrect->s0 = q.s0;
    charrect->s1 = q.s1;
    charrect->t0 = q.t0;
    charrect->t1 = q.t1;
}

#endif

#endif // MF_FONT_H
