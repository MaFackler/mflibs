#ifndef MF_PLATFORM_H
#define MF_PLATFORM_H
/* USAGE:

// To create window with opengl context
#define MF_PLATFORM_USE_OPENGL
// Use implementation also
#define MF_PLATFORM_IMPLEMENTATION
#include "mf_platform.h"


int main()
{
    mfp_platform platform = {};

    mfp_init(&platform);
    mfp_window_open(&platform, "Example", 0, 0, 1600, 900);

    bool running = true;
    while (running && platform.window.isOpen)
    {
        mfp_begin(&platform);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        mfp_end(&platform);
    }

    mfp_window_close(&platform);
    mfp_destroy(&platform);
}

*/

#ifdef __cplusplus
#define mf_inline inline
#else
#include <stdbool.h>
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309L
#endif // _POSIX_C_SOURCE
#define mf_inline
#endif // __cplusplus

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#ifdef _WIN32
#include <windows.h>
#endif // WIN32

#ifdef MF_PLATFORM_USE_OPENGL
#define GL_GLEXT_PROTOTYPES
//#define GLX_GLEXT_PROTOTYPES
//
#include <GL/gl.h>
#include <GL/glu.h>
#ifdef __linux__
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/glx.h>
#include <GL/glext.h>
#else // WINDOWS

#define GL_COMPILE_STATUS                 0x8B81
#define GL_VERTEX_SHADER                  0x8B31
#define GL_VALIDATE_STATUS                0x8B83
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_LINK_STATUS                    0x8B82
#define WGL_CONTEXT_MAJOR_VERSION_ARB           0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB           0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB            0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB        0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002

#define GL_FUNC_DEF(rtype, name, ...) \
    typedef rtype (*name##Proc)(__VA_ARGS__); \
    static name##Proc name;

GL_FUNC_DEF(HGLRC, wglCreateContextAttribsARB, HDC hDC, HGLRC hShareContext, const int *attribList);
GL_FUNC_DEF(GLuint, glCreateShader, GLenum type);
// TODO: why gl char not working
GL_FUNC_DEF(void, glShaderSource, GLuint shader, GLsizei count, char **string, GLint *length);
GL_FUNC_DEF(void, glCompileShader, GLuint shader);
GL_FUNC_DEF(void, glAttachShader, GLuint program, GLuint shader);
GL_FUNC_DEF(void, glGetShaderiv, GLuint shader, GLenum pname, GLint *params);
GL_FUNC_DEF(void, glGetShaderInfoLog, GLuint shader, GLsizei bufSIze, GLsizei *length, char *infoLog);
GL_FUNC_DEF(GLuint, glCreateProgram, void);
GL_FUNC_DEF(void, glLinkProgram, GLuint program);


#endif

#endif // MF_PLATFORM_USE_OPENGL

typedef uint32_t u32;
typedef uint64_t u64;
typedef int32_t i32;



typedef struct mfp_button_state mfp_button_state;
typedef struct mfp_platform mfp_platform;
typedef struct mfp_input mfp_input;
typedef struct mfp_timer mfp_timer;
typedef struct mfp_window mfp_window;

void mfp_init(mfp_platform *platform);
void mfp_destroy(mfp_platform *platform);

void mfp_begin(mfp_platform *platform);
void mfp_end(mfp_platform *platform, bool swapBuffers);

void mfp_window_toggle_fullscreen(mfp_platform *platform);
void mfp_window_open(mfp_platform *platform, const char *title, i32 x, i32 y, i32 width, i32 height);
void mfp_window_close(mfp_platform *platform);

void mfp__end(mfp_platform *platform);
u64 mfp__get_ticks();

struct mfp_button_state
{
    bool down;
    bool pressed;
    bool released;
};

#define MFP__AMOUNT_KEYS 256

struct mfp_input
{
    bool enableKeyRepeat;
    mfp_button_state keys[MFP__AMOUNT_KEYS];

    char downKeysBuffer[256];
    u32 downKeysBufferSize = 0;

    // Text input
    char text[256];
    u32 textLength;

    // Input
    float mouseWheelDelta;

    mfp_button_state mouseLeft;
    i32 mouseX;
    i32 mouseY;
};

struct mfp_window
{
    const char *title;
    bool isOpen;
    bool isFullscreen;

    // NOTE: top down
    i32 x;
    i32 y;
    u32 width;
    u32 height;
};

struct mfp_timer
{
    // Time
    u64 ticks;
    float deltaSec;
    float fps;
};

enum mfp_MessageType
{
    MFP_MESSAGE_WINDOW_SIZE,
};

typedef void (*mfp_MessageCallbackFunc)(mfp_MessageType);
struct mfp_platform
{
    void *os;
    mfp_MessageCallbackFunc callback;

    mfp_input input;
    mfp_window window;
    mfp_timer timer;
};


enum
{
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



#ifdef MF_PLATFORM_IMPLEMENTATION

// TODO: whats a good practice so i dont have to redefine this every time?
#define MFP_Assert(expr) if (!(expr)) {*(int *) 0 = 0; }
#define MFP_ArrayLength(arr) (sizeof(arr) / sizeof(arr[0]))

#include <stdio.h>
inline
void mfp__end(mfp_platform *platform)
{
    // reset input
    mfp_input *input = &platform->input;
    input->textLength = 0;
    input->text[0] = 0;
    input->downKeysBuffer[0] = 0;
    input->downKeysBufferSize = 0;
    input->mouseLeft.pressed = false;
    input->mouseWheelDelta = 0.0f;

#ifdef _WIN32
    static LARGE_INTEGER frequency;
    if (frequency.QuadPart == 0) {
        QueryPerformanceFrequency(&frequency);
    }
#endif

    // update time
    mfp_timer *timer = &platform->timer;
    u64 ticks = mfp__get_ticks();

#ifdef _WIN32
    timer->deltaSec = ((float) ticks - (float) timer->ticks) / (float) frequency.QuadPart;
#else
    timer->deltaSec = (float) (ticks - timer->ticks) / 1000.0f;
#endif
    timer->ticks = ticks;
    timer->fps = (1.0f / timer->deltaSec);
}

#ifdef __linux__
// LINUX PLATFORM

#ifdef MF_PLATFORM_USE_OPENGL
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, int, const int*);
#endif

typedef struct
{
    Display *display;
    int screen;
    Window root;
    Window window;
    //// NOTE visual is also a member of XVisualInfo in this case both members have to be set
    //// a application with OpenGl uses XVisualInfo and a xlib application just uses visual
    //// TODO: just use XVisualInfo
    XVisualInfo *vi;
    Visual *visual;
    Colormap colormap;
    i32 depth;

    void *graphicHandle;
} mfp_x11;


mf_inline
mfp_x11 *mfp__get_x11(mfp_platform *platform)
{
    return (mfp_x11 *) platform->os; 
}

u64 mfp__get_time_micro()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    u64 cycles = ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
    return cycles;
}

#ifdef MF_PLATFORM_USE_OPENGL
void mfp__init_opengl(mfp_platform *platform)
{
    mfp_x11 *x11 = mfp__get_x11(platform);
    static int visualAttribs[] = {
      GLX_X_RENDERABLE    , True,
      GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
      GLX_RENDER_TYPE     , GLX_RGBA_BIT,
      GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
      GLX_RED_SIZE        , 8,
      GLX_GREEN_SIZE      , 8,
      GLX_BLUE_SIZE       , 8,
      GLX_ALPHA_SIZE      , 8,
      GLX_DEPTH_SIZE      , 24,
      GLX_STENCIL_SIZE    , 8,
      GLX_DOUBLEBUFFER    , True,
      //GLX_SAMPLE_BUFFERS  , 1,
      //GLX_SAMPLES         , 4,
      None
    };
    i32 fbCount = 0;
    GLXFBConfig *fbc = glXChooseFBConfig(x11->display, x11->screen, visualAttribs, &fbCount);

    MFP_Assert(fbc);
    i32 fbIndex = -1;
    //i32 bestSampleBuffers = 0;
    i32 bestSamples = 0;

    for (i32 i = 0; i < fbCount; ++i)
    {
        XVisualInfo *vi = glXGetVisualFromFBConfig(x11->display, fbc[i]);
        i32 sampleBuffers = 0;
        i32 samples = 0;
        glXGetFBConfigAttrib(x11->display, fbc[i], GLX_SAMPLE_BUFFERS, &sampleBuffers);
        glXGetFBConfigAttrib(x11->display, fbc[i], GLX_SAMPLES, &samples);
        if (sampleBuffers && samples > bestSamples)
        {
            fbIndex = i;
            bestSamples = samples;
        }
        XFree(vi);
    }
    MFP_Assert(fbIndex >= 0);
    
    int attribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
        GLX_CONTEXT_MINOR_VERSION_ARB, 0,
        None
    };
    
    x11->vi = glXGetVisualFromFBConfig(x11->display, fbc[fbIndex]);
    x11->visual = x11->vi->visual;
    x11->colormap = XCreateColormap(x11->display,
                                        x11->root,
                                        x11->vi->visual,
                                        AllocNone);

    x11->graphicHandle = malloc(sizeof(GLXContext));


    glXCreateContextAttribsARBProc createContext = (glXCreateContextAttribsARBProc)glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB");
    MFP_Assert(createContext);
    //*((GLXContext *) x11->graphicHandle) = glXCreateContext(x11->display, x11->vi, 0, GL_TRUE);

    //*((GLXContext *) x11->graphicHandle) = glXCreateContextAttribsARBProc(x11->display, fbc[fbIndex], 0, True, attribs);
    GLXContext context = createContext(x11->display, fbc[fbIndex], 0, True, attribs);
    //GLXContext glxContext = glXCreateContext(x11->display, x11->vi, NULL, GL_TRUE);
    bool success = glXMakeCurrent(x11->display, x11->window, context);
    MFP_Assert(success);

    *((GLXContext *) x11->graphicHandle) = context;

    i32 major = 0;
    i32 minor = 0;
    glXQueryVersion(x11->display, &major, &minor);

    // TODO: version??
    //MF_Assert(major == 3);
}
#endif


void mfp_init(mfp_platform *platform)
{
    platform->os = malloc(sizeof(mfp_x11));
    mfp_x11 *plat = (mfp_x11 *) platform->os;
    plat->display = XOpenDisplay(NULL);
    plat->screen = DefaultScreen(plat->display);
    plat->root = RootWindow(plat->display, plat->screen);

    plat->depth = XDefaultDepth(plat->display, plat->screen);
    plat->visual = XDefaultVisual(plat->display, plat->screen);
    plat->colormap = XDefaultColormap(plat->display, plat->screen);
    
    mfp_timer *timer = &platform->timer;
#ifdef MF_WINDOWS
    timer->time = mfp__get_time_micro();
#endif

}


void mfp_window_open(mfp_platform *platform, const char *title, i32 x, i32 y, i32 width, i32 height)
{
    MFP_Assert(!platform->window.isOpen);
    mfp_x11 *x11 = mfp__get_x11(platform);
    platform->window.x = x;
    platform->window.y = y;
    platform->window.width = width;
    platform->window.height = height;
    platform->window.title = title;
    XSetWindowAttributes windowAttributes;
    windowAttributes.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
        StructureNotifyMask | PointerMotionMask | EnterWindowMask | LeaveWindowMask |
        ButtonPressMask | ButtonReleaseMask;

    windowAttributes.colormap = x11->colormap;

    // TODO: multi monitor setup: x coordinate is always relative root
    // window so the window will launch on the left monitor not the fouces one
    x11->window = XCreateWindow(x11->display,
                                x11->root,
                                platform->window.x,
                                platform->window.y,
                                platform->window.width,
                                platform->window.height,
                                0,
                                x11->depth,
                                InputOutput,
                                x11->visual,
                                CWColormap | CWEventMask,
                                &windowAttributes);


    XMapWindow(x11->display, x11->window);
    if (platform->window.title)
        XStoreName(x11->display, x11->window, platform->window.title); 

    platform->window.isOpen = true;
#ifdef MF_PLATFORM_USE_OPENGL
    mfp__init_opengl(platform);
#endif
}


void mfp__dispatch_key(mfp_input *input, mfp_button_state *state, bool down)
{
    state->pressed = !state->down && down;
    state->released = state->down && !down;
    state->down = down;
}

void mfp__dispatch_xkey(mfp_input *input, XKeyEvent *event, bool down)
{
    KeySym sym = XLookupKeysym(event, 0);
    if (sym >= XK_space && sym <= XK_asciitilde)
    {
        mfp_button_state *state = &input->keys[sym];
        mfp__dispatch_key(input, state, down);
        if (state->pressed)
        {
            // handle text input
            char buffer[16] = {};
            u32 amount = 0;
            KeySym key;
            amount = XLookupString(event, buffer, sizeof(buffer), &key, 0);
            memcpy(&input->text[input->textLength], buffer, amount);
            input->textLength += amount;
            MFP_Assert(input->textLength < 256);
            input->text[input->textLength] = 0;
        } else if (state->down) {
            input->downKeysBuffer[input->downKeysBufferSize++] = sym;
        }
    }
    else
    {
        char mysym = 0;
        switch (sym)
        {
            case XK_BackSpace:
                mysym = MF_KEY_BACKSPACE;
                break;
            case XK_Tab:
                mysym = MF_KEY_TAB;
                break;
            case XK_Return:
                mysym = MF_KEY_RETURN;
                break;
            case XK_Shift_L:
            case XK_Shift_R:
                mysym = MF_KEY_SHIFT;
                break;
            case XK_Control_L:
            case XK_Control_R:
                mysym = MF_KEY_CTRL;
                break;
            case XK_Alt_L:
            case XK_Alt_R:
                mysym = MF_KEY_ALT;
                break;
            case XK_Caps_Lock:
                mysym = MF_KEY_CAPS;
                break;
            case XK_Escape:
                mysym = MF_KEY_ESCAPE;
                break;
            case XK_Super_L:
            case XK_Super_R:
                mysym = MF_KEY_SUPER;
                break;
            case XK_Up:
                mysym = MF_KEY_UP;
                break;
            case XK_Down:
                mysym = MF_KEY_DOWN;
                break;
            case XK_Left:
                mysym = MF_KEY_LEFT;
                break;
            case XK_Right:
                mysym = MF_KEY_RIGHT;
                break;
            default:
                break;
        }
        if (mysym != 0)
        {
            mfp_button_state *state = &input->keys[(i32)mysym];
            mfp__dispatch_key(input, state, down);
        }
    }

}

u64 mfp__get_ticks() {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC_RAW, &now);
    u64 res = ((u64) now.tv_sec * 1000) + ((u64) now.tv_nsec / 1000000);
    return res;
}

void mfp_begin(mfp_platform *platform)
{
    if (platform->timer.ticks == 0)
    {
        platform->timer.ticks = mfp__get_ticks();
    }
    XEvent event;
    mfp_input *input = &platform->input;

    for (u32 i = 0; i < MFP_ArrayLength(input->keys); ++i)
    {
        mfp_button_state *state = &input->keys[i];
        state->pressed = false;
        state->released = false;
    }

    mfp_x11* x11 = mfp__get_x11(platform);

    while(XPending(x11->display))
    {
        XNextEvent(x11->display, &event);
        switch (event.type)
        {
            case KeyPress:
            {
                mfp__dispatch_xkey(input, &event.xkey, true);
            } break;
            case KeyRelease:
            {
                // NOTE: xserver sends release and press if key is hold down
                // this will ignore those events
                if (XEventsQueued(x11->display, QueuedAfterReading))
                {
                    XEvent next;
                    XPeekEvent(x11->display, &next);
                    if (next.type == KeyPress && next.xkey.time == event.xkey.time)
                    {
                        if (input->enableKeyRepeat)
                            mfp__dispatch_xkey(input, &event.xkey, false);
                        else
                            continue;
                    }
                } 
                mfp__dispatch_xkey(input, &event.xkey, false);
            } break;
            case ConfigureNotify:
            {
            //XConfigureEvent xce = event.xconfigure;

            // TODO: 
            //if (xce.width != input->width ||
            //    xce.height != input->height)
            //{
            //    input->width = xce.width;
            //    input->height = xce.height;
            //}
            } break;
            case EnterNotify:
            case LeaveNotify:
            case MotionNotify:
            {
                XMotionEvent xme = event.xmotion;
                input->mouseX = xme.x;
                input->mouseY = platform->window.height - xme.y;
            } break;
            case ButtonPress:
            {
                //XButtonEvent xbe = event.xbutton;
                input->mouseLeft.down = true;
            } break;
            case ButtonRelease:
            {
                input->mouseLeft.released = true;
                input->mouseLeft.pressed = input->mouseLeft.down;
                input->mouseLeft.down = false;
            } break;
        }

    }

}

void mfp_end(mfp_platform *platform, bool swapBuffers=true)
{
    mfp__end(platform);
#ifdef MF_PLATFORM_USE_OPENGL
    if (swapBuffers) {
        mfp_x11 *x11 = mfp__get_x11(platform);
        glXSwapBuffers(x11->display, x11->window);
    }
#endif
}


void mfp_window_close(mfp_platform *platform)
{
    mfp_window *window = &platform->window;
    mfp_x11 *x11 = mfp__get_x11(platform);
    XDestroyWindow(x11->display, x11->window);
    XCloseDisplay(x11->display);
    window->isOpen = false;
}

void mfp_window_toggle_fullscreen(mfp_platform *platform)
{
    MFP_Assert(!"TODO");
}

void mfp_destroy(mfp_platform *platform)
{
}

#else

static mfp_platform *g_platform = NULL;
typedef struct
{
    HWND window;
    HDC dc;

    void *graphicHandle;
} mfp_win;


u64 mfp__get_ticks()
{
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return counter.QuadPart;
}

mf_inline
mfp_win *mfp__get_win(mfp_platform *platform)
{
    return (mfp_win *) platform->os; 
}

void mfp_init(mfp_platform *platform)
{
    platform->os = malloc(sizeof(mfp_win));
}

void mfp_begin(mfp_platform *platform)
{

    if (platform->timer.ticks == 0)
    {
        platform->timer.ticks = mfp__get_ticks();
    }
    for (size_t i = 0; i < MFP__AMOUNT_KEYS; ++i)
    {
        mfp_button_state *state = &platform->input.keys[i];
        state->pressed = false;
        state->released = false;
    }

    mfp_win *os = mfp__get_win(platform);
    MSG msg;
    while (PeekMessage(&msg, os->window, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void mfp_end(mfp_platform *platform, bool swapBuffers=true)
{
    mfp__end(platform);
#ifdef MF_PLATFORM_USE_OPENGL
    if (swapBuffers) {
        mfp_win *os = mfp__get_win(platform);
        SwapBuffers(os->dc);
    }
#endif
}

void mfp_destroy(mfp_platform *platform)
{
    mfp_win *os = mfp__get_win(platform);
#ifdef MF_PLATFORM_USE_OPENGL
    HGLRC *hgl = (HGLRC *) os->graphicHandle;
    wglDeleteContext(*hgl);
#endif
    DestroyWindow(os->window);
    DeleteObject(os->dc);
}

void mfp_window_toggle_fullscreen(mfp_platform *platform)
{
    void *dummy = platform;
    assert(dummy);
}

mf_inline
void mfp__get_client_rect(HWND window, i32 *x, i32 *y, u32 *width, u32 *height)
{
    RECT clientRect;
    GetClientRect(window, &clientRect);

    *x = clientRect.left;
    *y = clientRect.top;
    *width = clientRect.right - clientRect.left;
    *height = clientRect.bottom - clientRect.top;
}

mf_inline
void mfp__set_mouse_pos(mfp_platform *platform, LPARAM lParam)
{
    i32 x = LOWORD(lParam);
    i32 y = platform->window.height - HIWORD(lParam);
    platform->input.mouseX = x;
    platform->input.mouseY = y;
}

void mfp__dispatch_key_to_input(mfp_input *input, mfp_button_state *state, bool down)
{
    state->pressed = !state->down && down;
    state->released = state->down && !down;
    if (input->enableKeyRepeat && state->down && down)
    {
        // NOTE: this will generate keyrepeate
        state->pressed = true;
    }
    state->down = down;
}

void mfp__dispatch_windows_key(mfp_input *input, u32 keycode, bool down)
{
    i32 keyIndex = keycode;
    char buf[1024] = {};
    OutputDebugString(buf);
    // NOTE: hack that you dont have to query keys with capital chars
    if (keyIndex >= 'a' && keyIndex <= 'z')
        return;
    if (keyIndex >= 'A' && keyIndex <= 'Z')
    {
        i32 upperLowerOffset = 32;
        if (!input->keys[MF_KEY_SHIFT].down)
        {
            keyIndex += upperLowerOffset;
        }
        else
        {
            // NOTE: if upper case also dispatch lower case press
            mfp_button_state *state = &input->keys[keyIndex + upperLowerOffset];
            mfp__dispatch_key_to_input(input, state, down);
        }
    }
    else if (keyIndex == VK_BACK)
        keyIndex = MF_KEY_BACKSPACE;
    else if (keyIndex == VK_TAB)
        keyIndex = MF_KEY_TAB;
    else if (keyIndex == VK_RETURN)
        keyIndex = MF_KEY_RETURN;
    else if (keyIndex == VK_SHIFT)
        keyIndex = MF_KEY_SHIFT; 
    else if (keyIndex == VK_CONTROL)
        keyIndex = MF_KEY_CTRL; 
    else if (keyIndex == VK_MENU)
        keyIndex = MF_KEY_ALT; 
    else if (keyIndex == VK_CAPITAL)
        keyIndex = MF_KEY_CAPS; 
    else if (keyIndex == VK_LWIN)
        keyIndex = MF_KEY_SUPER; 
    else if (keyIndex == VK_UP)
        keyIndex = MF_KEY_UP; 
    else if (keyIndex == VK_DOWN)
        keyIndex = MF_KEY_DOWN; 
    else if (keyIndex == VK_LEFT)
        keyIndex = MF_KEY_LEFT; 
    else if (keyIndex == VK_RIGHT)
        keyIndex = MF_KEY_RIGHT; 
    else if (keyIndex == VK_ESCAPE)
        keyIndex = MF_KEY_ESCAPE; 

    mfp_button_state *state = &input->keys[keyIndex];
    if (down) {
        input->downKeysBuffer[input->downKeysBufferSize++] = keyIndex;
    }
    mfp__dispatch_key_to_input(input, state, down);
}

LRESULT CALLBACK mfp__window_proc(HWND wnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT res = 0;
    //Platform *platform = (Platform *) GetWindowLongPtr(wnd, GWLP_USERDATA);
    assert(g_platform != NULL);
    mfp_win *os = mfp__get_win(g_platform);
    switch (message)
    {
        case WM_SIZE:
        {
            if (g_platform->callback)
            {
                mfp_window *window = &g_platform->window;
                mfp__get_client_rect(os->window,
                                     &window->x,
                                     &window->y,
                                     &window->width,
                                     &window->height);
                g_platform->callback(MFP_MESSAGE_WINDOW_SIZE);
            }
            break;
        }
        case WM_LBUTTONDOWN:
        {
            mfp__set_mouse_pos(g_platform, lParam);
            g_platform->input.mouseLeft.pressed = true;
            break;
        }
        case WM_MOUSEMOVE:
        {
            mfp__set_mouse_pos(g_platform, lParam);
        }
        case WM_MOUSEWHEEL:
        {
            //i32 keys = GET_KEYSTATE_WPARAM(wParam);
            i32 delta = GET_WHEEL_DELTA_WPARAM(wParam);
            g_platform->input.mouseWheelDelta = (float) delta;
            break;
        }
        case WM_CLOSE:
        case WM_DESTROY:
        case WM_QUIT:
        {
            g_platform->window.isOpen = false;
            break;
        }
        case WM_SYSKEYDOWN:
        case WM_KEYDOWN:
        {
            // TODO: if the _WinProc loop is slow the amount of repeats will be more than one
            // so the application misses some key repeats but maybe this is okay
            //u32 amount = lParam & 0x0FFFF;
            mfp__dispatch_windows_key(&g_platform->input, (u32) wParam, true);
            break;
        }
        case WM_SYSKEYUP:
        case WM_KEYUP:
        {
            mfp__dispatch_windows_key(&g_platform->input, (u32) wParam, false);
            break;
        }
        case WM_EXITSIZEMOVE:
        {
            i32 x, y;
            u32 width, height;
            mfp__get_client_rect(os->window, &x, &y, &width, &height);
            mfp_window *window = &g_platform->window;
            if (x != window->x ||
                y != window->y ||
                width != window->width ||
                height != window->height)
            {
                window->x = x;
                window->y = y;
                window->width = width;
                window->height = height;
            }
        }
        case WM_PAINT:
        {
            //PAINTSTRUCT Paint;
            //HDC dc = BeginPaint(wnd, &Paint);
            //EndPaint(wnd, &Paint);
            res = DefWindowProcA(wnd, message, wParam, lParam);
            break;
        }
        case WM_CHAR:
        {
            WCHAR utfChar = (WCHAR) wParam;
            char asciiChar;
            i32 length = WideCharToMultiByte(CP_ACP, 0, &utfChar, 1, &asciiChar, 1, 0, 0);
            mfp_input *input = &g_platform->input;
            if (length == 1 && asciiChar >= ' ')
            {
                if (asciiChar == '\r') {
                    asciiChar = '\n';
                }
                input->text[input->textLength++] = asciiChar;
                input->text[input->textLength] = 0;
            }
        }

        default:
            res = DefWindowProcA(wnd, message, wParam, lParam);
    }
    return res;
}

#ifdef MF_PLATFORM_USE_OPENGL
void mfp__init_opengl(mfp_platform *platform)
{
    mfp_win *os = mfp__get_win(platform);
    PIXELFORMATDESCRIPTOR pf = {};
    pf.nSize = sizeof(pf);
    pf.nVersion = 1;
    pf.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
    pf.cColorBits = 24;
    pf.cAlphaBits = 8;
    int suggestedIndex = ChoosePixelFormat(os->dc, &pf);
    PIXELFORMATDESCRIPTOR spf;
    DescribePixelFormat(os->dc, suggestedIndex, sizeof(spf), &spf);
    SetPixelFormat(os->dc, suggestedIndex, &spf);

    os->graphicHandle = malloc(sizeof(HGLRC));
    HGLRC *hgl = (HGLRC *) os->graphicHandle;
    HGLRC basicContext = wglCreateContext(os->dc);
    if (wglMakeCurrent(os->dc, basicContext))
    {
#define GL_FUNC_LOAD(name)\
    name = (name##Proc) wglGetProcAddress(#name);
        wglCreateContextAttribsARB = (wglCreateContextAttribsARBProc) wglGetProcAddress("wglCreateContextAttribsARB");
        //GL_FUNC_LOAD(wglCreateContextAttribsARB);
        GL_FUNC_LOAD(glCreateShader);
        GL_FUNC_LOAD(glShaderSource);
        GL_FUNC_LOAD(glCompileShader);
        GL_FUNC_LOAD(glAttachShader);
        GL_FUNC_LOAD(glGetShaderiv);
        GL_FUNC_LOAD(glGetShaderInfoLog);
        GL_FUNC_LOAD(glLinkProgram);
        GLint attribs[] =
        {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
            WGL_CONTEXT_MINOR_VERSION_ARB, 3,
            WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
            0
        };
        HGLRC modernContext = wglCreateContextAttribsARB(os->dc, 0, &attribs[0]);
        if (modernContext)
        {
            wglMakeCurrent(os->dc, modernContext);
            wglDeleteContext(basicContext);
            *hgl = modernContext;
        }
        else
        {
            *hgl = basicContext;
        }
    }
}
#endif // MF_PLATFORM_USE_OPENGL

void mfp_window_open(mfp_platform *platform, const char *title, i32 x, i32 y, i32 width, i32 height)
{
    WNDCLASS wc = {};

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = mfp__window_proc;
    wc.lpszClassName = "Platform";
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    RegisterClass(&wc);

    // TODO: this is crap
    g_platform = platform;


    mfp_win *os = mfp__get_win(platform);
    mfp_window *window = &platform->window;
    window->x = x;
    window->y = y;
    window->title = title;

    RECT sizeToRequest = {};
    sizeToRequest.left = 0;
    sizeToRequest.right = width;
    sizeToRequest.top = 0;
    sizeToRequest.bottom = height;
    DWORD style = (WS_OVERLAPPEDWINDOW | WS_VISIBLE) & ~WS_THICKFRAME;
    AdjustWindowRect(&sizeToRequest, style, false);
    os->window = CreateWindowA(wc.lpszClassName,
                               window->title,
                               style,
                               window->x, window->y,
                               sizeToRequest.right - sizeToRequest.left,
                               sizeToRequest.bottom - sizeToRequest.top,
                               0, 0, 0, 0);
    assert(os->window);
    os->dc = GetDC(os->window);

    PIXELFORMATDESCRIPTOR desiredPixelFormat = {};
    desiredPixelFormat.nSize = sizeof(desiredPixelFormat);
    desiredPixelFormat.nVersion = 1;
    desiredPixelFormat.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
    desiredPixelFormat.cColorBits = 32;
    desiredPixelFormat.cAlphaBits = 8;
    desiredPixelFormat.iLayerType = PFD_MAIN_PLANE;

    // TODO: pixelformat always needed??
    i32 pixelFormatIndex = ChoosePixelFormat(os->dc, &desiredPixelFormat);
    PIXELFORMATDESCRIPTOR suggestedPixelFormat;
    DescribePixelFormat(os->dc, pixelFormatIndex, sizeof(suggestedPixelFormat), &suggestedPixelFormat);
    SetPixelFormat(os->dc, pixelFormatIndex, &suggestedPixelFormat);

    // TODO: client rect

    mfp__get_client_rect(os->window, &window->x, &window->y, &window->width, &window->height);
    window->isOpen = true;
#ifdef MF_PLATFORM_USE_OPENGL
    mfp__init_opengl(platform);
#endif
}

void mfp_window_close(mfp_platform *platform)
{
    assert(platform);
}


#endif

#endif // MF_PLATFORM_IMPLEMENTATION
#endif // MF_PLATFORM_H

