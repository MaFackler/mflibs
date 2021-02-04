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
    while (running)
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

#include <string.h>
#include <stdlib.h>
#include <time.h>
#ifdef __linux__
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#else // WINDOWS
#include <windows.h>
#endif

#ifdef MF_PLATFORM_USE_OPENGL
//#define GL_GLEXT_PROTOTYPES
//#define GLX_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glx.h>
//#include <GL/glu.h>
//#include <GL/glxext.h>
//#include <GL/glext.h>
#endif

typedef u_int32_t u32;
typedef u_int64_t u64;
typedef int32_t i32;

typedef struct mfp_button_state mfp_button_state;
typedef struct mfp_platform mfp_platform;
typedef struct mfp_input mfp_input;
typedef struct mfp_timer mfp_timer;

void mfp_init(mfp_platform *platform);
void mfp_destroy(mfp_platform *platform);

void mfp_begin(mfp_platform *platform);
void mfp_end(mfp_platform *platform);

void mfp_window_toggle_fullscreen(mfp_platform *platform);
void mfp_window_open(mfp_platform *platform, const char *title, i32 x, i32 y, i32 width, i32 height);
void mfp_window_close(mfp_platform *platform);


struct mfp_button_state
{
    bool down = false;
    bool pressed = false;
    bool released = false;
};

struct mfp_input
{
    bool enableKeyRepeat = true;
    mfp_button_state keys[256] = {};

    // Text input
    char text[256] = {};
    u32 textLength = 0; 

    // Input
    float mouseWheelDelta = 0.0f;

    mfp_button_state mouseLeft;
    i32 mouseX = 0;
    i32 mouseY = 0;
};

struct mfp_window
{
    const char *title = NULL;
    bool isOpen = false;
    bool isFullscreen = false;

    // NOTE: top down
    i32 x = 0;
    i32 y = 0; 
    u32 width = 1600;
    u32 height = 900;
};

struct mfp_timer
{
    // Time
    u64 time = 0;
    float deltaSec = 0.0f;
    u64 deltaMicroSec = 0;
    u64 deltaMilliSec = 0;
    u32 fps = 0;
};

struct mfp_platform
{
    void *os = nullptr;

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

#ifdef __linux__
// LINUX PLATFORM

typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, int, const int*);

typedef struct
{
    Display *display;
    int screen;
    Window root;
    Window window;
    // NOTE visual is also a member of XVisualInfo in this case both members have to be set
    // a application with OpenGl uses XVisualInfo and a xlib application just uses visual
    // TODO: just use XVisualInfo
    XVisualInfo *vi;
    Visual *visual;
    Colormap colormap;
    i32 depth;

    void *graphicHandle;
} mfp_x11;

inline
mfp_x11 *mfp__get_x11(mfp_platform *platform)
{
    return (mfp_x11 *) platform->os; 
}

inline
u64 mfp__get_time_micro()
{
    timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    u64 cycles = time.tv_sec * 1000000 + time.tv_nsec / 1000;
    return cycles;
}

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
    timer->time = mfp__get_time_micro();

}


void mfp_window_open(mfp_platform *platform, const char *title, i32 x, i32 y, i32 width, i32 height)
{
    MFP_Assert(!platform->window.isOpen);
    mfp_x11 *x11 = mfp__get_x11(platform);
    platform->window.x = x;
    platform->window.y = y;
    platform->window.width = width;
    platform->window.height = height;
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

void mfp_begin(mfp_platform *platform)
{
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

void mfp_end(mfp_platform *platform)
{
    // reset input
    mfp_input *input = &platform->input;
    input->textLength = 0;
    input->text[0] = 0;
    input->mouseLeft.pressed = false;

    // update time
    mfp_timer *timer = &platform->timer;
    u64 time = mfp__get_time_micro();
    timer->deltaMicroSec = time - timer->time;
    timer->deltaMilliSec = timer->deltaMicroSec / 1000;
    timer->deltaSec = timer->deltaMilliSec / 1000.0f;
    timer->time = time;
    if (timer->deltaMilliSec > 0)
        timer->fps = 1000 / timer->deltaMilliSec;
        
#ifdef MF_PLATFORM_USE_OPENGL
    mfp_x11 *x11 = mfp__get_x11(platform);
    glXSwapBuffers(x11->display, x11->window);
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
#endif

#endif // MF_PLATFORM_IMPLEMENTATION
#endif // MF_PLATFORM_H

