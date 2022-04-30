#ifndef MF_FONT_H
#define MF_FONT_H

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

typedef struct {
    float x0, x1, y0, y1;
    float s0, s1, t0, t1;
} mffo_charrect;

typedef struct {
    unsigned char *data;
    size_t dim;
    mffo_charrect charrects[256];
} mffo_font;


void mffo_font_alloc(mffo_font *font, const char *filename);

#define _MF_FONT_START_CHAR 32
#define _MF_FONT_END_CHAR 128

#ifdef MF_FONT_IMPLEMENTATION

void mffo_font_alloc(mffo_font *font, const char *filename) {
    const size_t dim = 512;
    unsigned char file_contents[1<<20];
    unsigned char alpha_font[dim * dim];
    font->data = (unsigned char*) malloc(dim * dim * sizeof(unsigned int));
    fread(file_contents, 1, 1<<20, fopen(filename, "rb"));
    font->dim = dim;
#if 0 
    stbtt_BakeFontBitmap(file_contents,
                         0, 32.0,
                         alpha_font, dim, dim,
                         _MF_FONT_START_CHAR, _MF_FONT_END_CHAR - _MF_FONT_START_CHAR, font->chardata);


    unsigned int *dest = (unsigned int *) font->data;
    for (size_t i = 0; i < dim * dim; ++i) {
        char alpha_value = alpha_font[i];
        unsigned int value = (alpha_value << 24 |
                              alpha_value << 16 |
                              alpha_value << 8 |
                              alpha_value << 0);
        dest[i] = value;
    }
    font->dim = dim;
#endif
    stbtt_fontinfo stbf;
    stbtt_InitFont(&stbf, file_contents, stbtt_GetFontOffsetForIndex(file_contents, 0));

    int height = 64;
    int xdest = 0;
    int ydest = 0;
    for (char c = '!'; c <= '~'; ++c) {
        int w, h, dx, dy;
        unsigned char *cbitmap = stbtt_GetCodepointBitmap(&stbf, 0,
                                                          stbtt_ScaleForPixelHeight(&stbf, height),
                                                          c,
                                                          &w, &h, &dx, &dy);

        printf("got %c: w=%d, h=%d, dx=%d, dy=%d\n", c, w, h, dx, dy);

        if (xdest + w > font->dim) {
            xdest = 0;
            ydest += height;
            assert(ydest < font->dim);
        }

        unsigned int *dest_row = (unsigned int *) font->data;
        dest_row += (font->dim * (h + ydest - 1));
        for (int y = 0; y < h; ++y) {

            unsigned int *dest = dest_row + xdest;
            for (int x = 0; x < w; ++x) {
                char alpha_value = cbitmap[(y * w) + x];
                unsigned int value = (alpha_value << 24 |
                                      alpha_value << 16 |
                                      alpha_value << 8 |
                                      alpha_value << 0);
                *dest++ = value;
                //*dest++ = 0xFFFFFFFF;
            }
            dest_row -= font->dim;
        }
        stbtt_FreeBitmap(cbitmap, 0);

        // fill meta data
        font->charrects[c].x0 = 0.0f;
        font->charrects[c].x1 = dx + w;
        font->charrects[c].y0 = -(-h - dy);
        font->charrects[c].y1 = -(-h - dy) + h;

        font->charrects[c].s0 = (float) (xdest) / (float) font->dim;
        font->charrects[c].s1 = (float) (xdest + w) / (float) font->dim;

        font->charrects[c].t0 = (float) (ydest) / (float) font->dim;
        font->charrects[c].t1 = (float) (ydest + h) / (float) font->dim;

        xdest += w;
    }
                                                      
}

bool mffo_is_supported_char(mffo_font *font, char c) {
    return (c >= _MF_FONT_START_CHAR && c < _MF_FONT_START_CHAR);
}

void mffo_font_get_charrect(mffo_font *font, mffo_charrect *charrect, char c) {
#if 0
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
#endif
}

#endif

#endif // MF_FONT_H
