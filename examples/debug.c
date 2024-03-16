#define MF_PLATFORM_USE_OPENGL
#define MF_PLATFORM_IMPLEMENTATION
#include "mf_platform.h"
#define MF_FONT_IMPLEMENTATION
#include "mf_font.h"

void draw_text(mffo_font *font, const char *text) {

    float x = 0.0f;
    float y = 0.0f;
#if 0
    while (char c = *text++) {
        auto cr = font->charrects[c];
        glBegin(GL_QUADS);
            glTexCoord2f(cr.s0, cr.t0);
            //glTexCoord2f(0.0f, 0.0f);
            //glVertex2f(-1.0f, -1.0f);
            glVertex2f(x + cr.x0, cr.y0);

            glTexCoord2f(cr.s1, cr.t0);
            //glTexCoord2f(right, 0.0f);
            //glVertex2f(1.0f, -1.0f);
            glVertex2f(x + cr.x1, cr.y0);

            glTexCoord2f(cr.s1, cr.t1);
            //glTexCoord2f(right, top);
            //glVertex2f(1.0f, 1.0f);
            glVertex2f(x + cr.x1, cr.y1);

            glTexCoord2f(cr.s0, cr.t1);
            //glTexCoord2f(0.0f, top);
            //glVertex2f(-1.0f, 1.0f);
            glVertex2f(x + cr.x0, cr.y1);
        glEnd();

        x += cr.x1 - cr.x0;
    }
#endif
}


int main() {

    const int width = 1600;
    const int height = 900;
    mfp_platform p = {};
    mfp_init(&p);
    mfp_window_open(&p, "Debug", 0, 0, width, height);

    mffo_font font;
#ifdef _WIN32
    const char *path = "c:/windows/fonts/arialbd.ttf";
#else
    const char *path = "/usr/share/fonts/ubuntu/Ubuntu-B.ttf";
#endif
    mffo_font_init(&font, path, 64.0f);
    //mffo_font_alloc(&font, "/usr/share/fonts/TTF/FiraSans-Bold.ttf");
    
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    unsigned int id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
                 FONT_ATLAS_DIM, FONT_ATLAS_DIM, 0, GL_BGRA,
                 GL_UNSIGNED_BYTE, font.data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

#if 0
    mffo_charrect cr;
    mffo_font_get_charrect(&font, &cr, 'd');
#endif

    bool quit = false;


#if 0
    glMatrixMode(GL_PROJECTION);
    float mat[] =
    {
        2.0f/width, 0, 0, 0,
        0, 2.0f/height, 0, 0,
        0, 0, 1, 0,
        -1, -1, 0, 1
    };
    glLoadMatrixf(mat);
#endif

    while (!quit && p.window.isOpen) {
        mfp_begin(&p);

        if (p.input.keys['q'].pressed) {
            quit = true;
        }

        glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindTexture(GL_TEXTURE_2D, id);
        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f);
            glVertex2f(-1.0f, -1.0f);
            glTexCoord2f(1.0f, 0.0f);
            glVertex2f(1.0f, -1.0f);
            glTexCoord2f(1.0f, 1.0f);
            glVertex2f(1.0f, 1.0f);
            glTexCoord2f(0.0f, 1.0f);
            glVertex2f(-1.0f, 1.0f);
        glEnd();

        //draw_text(&font, "hellop'");
        mfp_end(&p, true);
    }

    mfp_window_close(&p);
    mfp_destroy(&p);
}
