#ifndef MF_FONT_H
#define MF_FONT_H

#include <assert.h>
#include "mf.h"
#ifdef _WIN32
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>
#else
#include <ft2build.h>
#include FT_FREETYPE_H
#endif

typedef struct {
    float width;
    float height;
    float xbearing;
    float ybearing;
    float advance;
    float u0;
    float u1;
    float v0;
    float v1;
} mffo_font_char;

typedef struct {
    float ascent;
    float descent;
    float linegap;
    unsigned char *data;
    mffo_font_char characters[256];
} mffo_font;

#define FONT_ATLAS_DIM 1024

// TODO: memory
static char ttf_buffer[1<<25];

int mffo_font_init(mffo_font *font, const char *path, float size);

#ifdef _WIN32
#else
void mffo__font_load_chars(mffo_font *font, FT_Face &face);
#endif


#ifdef MF_FONT_IMPLEMENTATION
#ifdef _WIN32
int mffo_font_init(mffo_font *font, const char *path, float size) {
    stbtt_fontinfo stbfontinfo;
    const unsigned char *ptr = (const unsigned char *) &ttf_buffer[0];

    int w, h, xoffset, yoffset;
    font->data = (unsigned char *) malloc(FONT_ATLAS_DIM * FONT_ATLAS_DIM * sizeof(int));
    fread(ttf_buffer, 1, 1<<25, fopen(path, "rb"));

    int offset = stbtt_GetFontOffsetForIndex(ptr, 0);
    stbtt_InitFont(&stbfontinfo, ptr, offset);

    int xatlas = 0;
    int yatlas = 0;
    int max_height = 0;
    memset(font->data, 100, FONT_ATLAS_DIM * FONT_ATLAS_DIM * sizeof(int));
    float scale = stbtt_ScaleForPixelHeight(&stbfontinfo, size);
    int ascent, descent, linegap;
    stbtt_GetFontVMetrics(&stbfontinfo, &ascent, &descent, &linegap);
    font->ascent = ascent * scale;
    font->descent = descent * scale;
    font->linegap = (float) linegap * scale;
    for (unsigned char c = 0; c < 128; c++) {
        // load character glyph 
        unsigned char *bitmap;
        bitmap = stbtt_GetCodepointBitmap(&stbfontinfo,
                                          0,
                                          scale,   
                                          c, &w, &h, &xoffset, &yoffset);
        int advance_width, lsb;
        stbtt_GetCodepointHMetrics(&stbfontinfo, c, &advance_width, &lsb);
        mffo_font_char character = {
            (float) w, (float) h,
            (float) (xoffset), (float) (h + yoffset),
            (float) (advance_width * scale),
        };
        max_height = (int) MF_Max(character.height, max_height);
        if (xatlas + character.width >= FONT_ATLAS_DIM) {
            xatlas = 0; 
            yatlas += max_height;
        }
        MF_Assert(yatlas + character.height < FONT_ATLAS_DIM);
        unsigned char *src = bitmap;

        for (int y = 0; y < character.height; ++y) {
            int *dest = (int*) font->data;
            dest += (FONT_ATLAS_DIM - yatlas - y - 1) * FONT_ATLAS_DIM;
            dest += xatlas;
            for (int x = 0; x < character.width; ++x) {
                char src_value = *src++;
                *dest++ = ((src_value << 24) |
                           (255 << 16) |
                           (255 << 8) |
                           (255 << 0));
            }
        }
        character.u0 = (float) xatlas / (float) FONT_ATLAS_DIM,
        character.u1 = (float) (xatlas + character.width) / (float) FONT_ATLAS_DIM,
        //character.v0 = (float) yatlas / (float) FONT_ATLAS_DIM,
        //character.v1 = (float) (yatlas + character.height) / (float) FONT_ATLAS_DIM,
        character.v1 = (float) (FONT_ATLAS_DIM - yatlas) / (float) FONT_ATLAS_DIM,
        character.v0 = (float) (FONT_ATLAS_DIM - yatlas - character.height) / (float) FONT_ATLAS_DIM,
        font->characters[c] = character;
        xatlas += (int) character.width;
    }
    return 0;
}
#else
int mffo_font_init(mffo_font *font, const char *path, float size) {
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        return -1;
    }
    FT_Face face;
    if (FT_New_Face(ft, path, 0, &face)) {
        return -1;
    }
    FT_Set_Pixel_Sizes(face, 0, size);
    font->ascent = face->size->metrics.ascender / 64.0f;
    font->descent = face->size->metrics.descender / 64.0f;
    font->linegap = font->ascent - font->descent;

    font->data = (unsigned char *) malloc(FONT_ATLAS_DIM * FONT_ATLAS_DIM * sizeof(int));

    mffo__font_load_chars(font, face);
    return 0;
}

void mffo__font_load_chars(mffo_font *font, FT_Face &face) {
    int xatlas = 0;
    int yatlas = 0;
    int max_height = 0;
    for (unsigned char c = 0; c < 128; c++) {
        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            continue;
        }
        assert(face->glyph->bitmap.pitch >= 0);
        mffo_font_char character = {
            face->glyph->bitmap.width, face->glyph->bitmap.rows,
            face->glyph->bitmap_left, face->glyph->bitmap_top,
            face->glyph->advance.x,
        };
        max_height = MF_Max(character.height, max_height);
        if (xatlas + character.width >= FONT_ATLAS_DIM) {
            xatlas = 0; 
            yatlas += max_height;
        }
        MF_Assert(yatlas + character.height < FONT_ATLAS_DIM);
        unsigned char *src = face->glyph->bitmap.buffer;

        for (int y = 0; y < character.height; ++y) {
            unsigned char *dest = font->data + ((yatlas + y) * FONT_ATLAS_DIM) + xatlas;
            for (int x = 0; x < character.width; ++x) {
                *dest++ = *src++;
            }
        }
        character.u0 = (float) xatlas / (float) FONT_ATLAS_DIM,
        character.u1 = (float) (xatlas + character.width) / (float) FONT_ATLAS_DIM,
        character.v0 = (float) yatlas / (float) FONT_ATLAS_DIM,
        character.v1 = (float) (yatlas + character.height) / (float) FONT_ATLAS_DIM,
        font->characters[c] = character;
        xatlas += character.width;
    }
}
#endif // _WIN32

#endif // MF_FONT_IMPLEMENTATION

#endif // MF_FONT_H
