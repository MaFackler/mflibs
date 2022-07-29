#pragma once
#include <mf.h>


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
#else
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#endif // WIN32

#ifdef MF_PLATFORM_USE_OPENGL
#ifdef __linux__
#include <GL/glx.h>
#include <GL/glext.h>
#else // WINDOWS


#endif

#endif // MF_PLATFORM_USE_OPENGL

namespace mf { namespace platform {

u64 _get_ticks();

// TODO: remove
//void mfp__end(mfp_platform *platform);
//u64 _get_ticks();




#ifdef MF_PLATFORM_IMPLEMENTATION

// TODO: whats a good practice so i dont have to redefine this every time?
#define MFP_Assert(expr) if (!(expr)) {*(int *) 0 = 0; }
#define MFP_ArrayLength(arr) (sizeof(arr) / sizeof(arr[0]))

#include <stdio.h>

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


void mfp__dispatch_key(mfp_input *input, ButtonState *state, bool down)
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
        ButtonState *state = &input->keys[sym];
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
            ButtonState *state = &input->keys[(i32)mysym];
            mfp__dispatch_key(input, state, down);
        }
    }

}


u64 _get_ticks() {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC_RAW, &now);
    u64 res = ((u64) now.tv_sec * 1000) + ((u64) now.tv_nsec / 1000000);
    return res;
}
void mfp_begin(mfp_platform *platform)
{
    if (platform->timer.ticks == 0)
    {
        platform->timer.ticks = _get_ticks();
    }
    XEvent event;
    mfp_input *input = &platform->input;

    for (u32 i = 0; i < MFP_ArrayLength(input->keys); ++i)
    {
        ButtonState *state = &input->keys[i];
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
    mfp_x11 *x11 = mfp__get_x11(platform);
#if 0
    Atom atoms[2] = {0};
    atoms[0] = XInternAtom(x11->display, "_NET_WM_STATE", false);
    atoms[1] = XInternAtom(x11->display, "_NET_WM_STATE_FULLSCREEN", false);

    XChangeProperty(x11->display,
                    x11->window,
                    atoms[0],
                    XA_ATOM, 32,
                    PropModeReplace, (unsigned char *) &atoms[0], 1);
#endif
    XEvent e;
    e.xclient.type = ClientMessage;
    e.xclient.window = x11->window;
    e.xclient.message_type = XInternAtom(x11->display, "_NET_WM_STATE", true);
    e.xclient.format = 32;
    e.xclient.data.l[0] = 2;
    e.xclient.data.l[1] = XInternAtom(x11->display, "_NET_WM_STATE_FULLSCREEN", true);
    e.xclient.data.l[2] = 0;
    e.xclient.data.l[3] = 1;
    e.xclient.data.l[4] = 0;
    XSendEvent(x11->display, x11->root, False, SubstructureRedirectMask | SubstructureNotifyMask, &e);
    // TODO: reset width and height?
    //XMoveResizeWindow(x11->display, x11->window, 0, 0, platform->window.width, platform->window.height);
}

void mfp_destroy(mfp_platform *platform)
{
}

#else

struct PlatformWindows {
} ;



PlatformWindows *mfp__get_win(Platform *platform)
{
    return (PlatformWindows *) platform->os; 
}

void mfp_init(Platform *platform)
{
    platform->os = malloc(sizeof(PlatformWindows));
}

void mfp_begin(Platform *platform)
{

}


void mfp_window_toggle_fullscreen(Platform *platform)
{
    void *dummy = platform;
    assert(dummy);
}

mf_inline






void mfp_window_open(mfp_platform *platform, const char *title, i32 x, i32 y, i32 width, i32 height)
{
}

void mfp_window_close(mfp_platform *platform)
{
    assert(platform);
}


#endif

#endif // MF_PLATFORM_IMPLEMENTATION

}}
