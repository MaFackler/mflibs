#ifndef MF_FONT_H
#define MF_FONT_H

#include <ft2build.h>
#include FT_FREETYPE_H

typedef struct {
    unsigned int width;
    unsigned int height;
    int xbearing;
    int ybearing;
    long int advance;
    float u0;
    float u1;
    float v0;
    float v1;
} mffo_font_char;

typedef struct {
    int ascent;
    int descent;
    int linegap;
    unsigned char *data;
    mffo_font_char characters[256];
} mffo_font;

#define FONT_ATLAS_DIM 512

int mffo_font_init(mffo_font *font, const char *path, float size);

void mffo__font_load_chars(mffo_font *font, FT_Face &face);

#ifdef MF_FONT_IMPLEMENTATION


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

    font->data = (unsigned char *) malloc(FONT_ATLAS_DIM * FONT_ATLAS_DIM);

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
        //character.data = (unsigned char *) malloc(character.width * character.height);
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

#endif // MF_FONT_IMPLEMENTATION

#endif // MF_FONT_H
