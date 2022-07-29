#pragma once
#include <mf.h>

namespace mf { namespace platform {

enum {
    MF_KEY_BACKSPACE = 1,
    MF_KEY_TAB,
    MF_KEY_RETURN,
    MF_KEY_SHIFT,
    MF_KEY_CTRL,
    MF_KEY_ALT,
    MF_KEY_CAPS,
    MF_KEY_SUPER,

    MF_KEY_UP,
    MF_KEY_DOWN,
    MF_KEY_RIGHT,
    MF_KEY_LEFT,
    MF_KEY_ESCAPE = 0x1b,
};

struct Window {
    const char *title;
    bool isOpen;
    bool isFullscreen;

    // NOTE: top down
    i32 x;
    i32 y;
    u32 width;
    u32 height;
};

struct ButtonState {
    bool down;
    bool pressed;
    bool released;
};

#define MFP__AMOUNT_KEYS 256

struct Input {
    bool enableKeyRepeat;
    ButtonState keys[MFP__AMOUNT_KEYS];

    char downKeysBuffer[256];
    u32 downKeysBufferSize = 0;

    // Text input
    char text[256];
    u32 textLength;

    // Input
    float mouseWheelDelta;

    ButtonState mouseLeft;
    i32 mouseX;
    i32 mouseY;
};

struct Timer {
    // Time
    u64 ticks;
    float deltaSec;
    float fps;
};


enum mfp_MessageType {
    MFP_MESSAGE_WINDOW_SIZE,
};
typedef void (*mfp_MessageCallbackFunc)(mfp_MessageType);

typedef void (*FuncCustomEnd)(void *);
struct IPlatform {
    void *os;
    mfp_MessageCallbackFunc callback;

    Input input;
    Timer timer;
    Window window;

    virtual void init() = 0;
    virtual void destroy() = 0;

    virtual void window_create(const char *title, i32 x, i32 y, i32 w, i32 h) = 0;
    virtual void window_close() = 0;

    virtual void begin() = 0;
    virtual void end() = 0;

    // NOTE: custom end routine which will be called by end()
    // needed for graphic apis
    FuncCustomEnd custom_end = NULL;
};

}}

#if defined(MF_OS_WINDOWS)
#include <mf_platform/windows/windows_platform.h>
#else
#include <mf_platform/linux/linux_platform.h>
#endif

