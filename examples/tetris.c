#define MF_IMPLEMENTATION
#include <mf.h>
#include <mf_math.h>
#define MF_PLATFORM_USE_OPENGL
#include <mf_platform.h>
#include <mf_opengl2.h>

typedef enum GamePhase {
    GAME_RUNNING,
    GAME_OVER
} GamePhase;

typedef struct Piece {

} Piece;

typedef struct GameState {
    GamePhase phase;
    v2i dim;
    u32 block_size;
    u8 *board;
    f32 delta_s;
    f32 tick;
    u32 lines_deleted;

    struct {
        u32 index;
        u32 count;
        Piece *data;
    } pieces;

    u8 direction;
    v2i piece_pos;
} GameState;

int main(int argc, char **argv) {

    mfp_platform platform = {0};
    mfp_init(&platform);
    mfp_window_open(&platform, "Example", 0, 0, 1600, 900);
    bool running = true;
    while (running && platform.window.isOpen) {
        mfp_begin(&platform);
        mfgl_set_color_rgb(255, 0, 0);
        mfgl_clear();
        mfp_end(&platform, true);
    }
    mfp_window_close(&platform);
    mfp_destroy(&platform);
}

int main(int argc, char **argv) {
    mfPlatform platform = {0};
    mfPlatformInit(&platform);
}




