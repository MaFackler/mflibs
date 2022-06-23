#ifndef MF_FONT_H
#define MF_FONT_H

#include <ft2build.h>
#include FT_FREETYPE_H

typedef struct {
    unsigned char *data;
    unsigned int width;
    unsigned int height;
    int xbearing;
    int ybearing;
    long int advance;
} mffo_font_char;

typedef struct {
    int ascent;
    int descent;
    int linegap;
    mffo_font_char characters[256];
} mffo_font;


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

    mffo__font_load_chars(font, face);
    return 0;
}

void mffo__font_load_chars(mffo_font *font, FT_Face &face) {

    for (unsigned char c = 0; c < 128; c++) {
        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            continue;
        }
        mffo_font_char character = {
            face->glyph->bitmap.buffer,     
            face->glyph->bitmap.width, face->glyph->bitmap.rows,
            face->glyph->bitmap_left, face->glyph->bitmap_top,
            face->glyph->advance.x
        };
        assert(face->glyph->bitmap.pitch >= 0);
        character.data = (unsigned char *) malloc(character.width * character.height);
        memcpy(character.data, face->glyph->bitmap.buffer, character.width * character.height);
        font->characters[c] = character;
    }
}

#endif // MF_FONT_IMPLEMENTATION

#endif // MF_FONT_H
