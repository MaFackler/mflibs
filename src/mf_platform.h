#ifndef MF_PLATFORM_H
#define MF_PLATFORM_H

// {{{ Includes

#ifdef __cplusplus
#else
#include <stdbool.h>
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309L
#endif // _POSIX_C_SOURCE
#endif // __cplusplus

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#endif // WIN32

// }}}

// {{{ Declarations

#define API static inline
#define MFP_WINDOWPOS_CENTER -1

typedef struct MFP_ButtonState MFP_ButtonState;
typedef struct MFP_Platform MFP_Platform;
typedef struct MFP_Input MFP_Input;
typedef struct MFP_Timer MFP_Timer;
typedef struct MFP_Window MFP_Window;

API void MFP_Init(MFP_Platform *platform);
API void MFP_SetTargetFps(MFP_Platform *platform, int fps);
API void MFP_Destroy(MFP_Platform *platform);
API void MFP_Begin(MFP_Platform *platform);
API void MFP_End(MFP_Platform *platform, bool swapBuffers);
API void MFP_WindowOpen(MFP_Platform *platform, const char *title, int x, int y, int width, int height);
API void MFP_WindowClose(MFP_Platform *platform);
API bool MFP_WindowShouldClose(MFP_Platform *platform);
API void MFP_WindowToggleFullscreen(MFP_Platform *platform);
API bool MFP_IsKeyPressed(MFP_Platform *platform, char c);
API bool MFP_IsKeyDown(MFP_Platform *platform, char c);
API bool MFP_IsKeyReleased(MFP_Platform *platform, char c);
API float MFP_GetDeltaSec(MFP_Platform *platform);
API double MFP_GetTimeSec(MFP_Platform *platform);
API void MFP_SleepMs(long ms);

API void MFP__End(MFP_Platform *platform);
API unsigned long int MFP__GetTicks();

struct MFP_ButtonState {
    bool down;
    bool pressed;
    bool released;
};

#define MFP__AMOUNT_KEYS 256

struct MFP_Input {
    bool enableKeyRepeat;
    MFP_ButtonState keys[MFP__AMOUNT_KEYS];

    char downKeysBuffer[256];
    size_t downKeysBufferSize;

    // Text input
    char text[256];
    size_t textLength;

    // Input
    float mouseWheelDelta;

    MFP_ButtonState mouseLeft;
    int mouseX;
    int mouseY;
    int mouseDx;
    int mouseDy;
};

struct MFP_Window {
    const char *title;
    bool isOpen;
    bool isFullscreen;

    // NOTE: top down
    int x;
    int y;
    size_t width;
    size_t height;
};

struct MFP_Timer {
    unsigned long int ticks;
    float timeSec;
    float deltaSec;
    float fps;
};

struct MFP_Platform {
    void *os;
    void (*graphicsAfterWindow)(MFP_Platform *platform);
    void (*graphicsBegin)(MFP_Platform *platform);
    void (*graphicsEnd)(MFP_Platform *platform);

    int targetFps;
    MFP_Input input;
    MFP_Window window;
    MFP_Timer timer;
};

typedef enum MFP_Keys {
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
} MFP_Keys;

// }}}

#if defined(MF_PLATFORM_IMPLEMENTATION) || defined(MF_IMPLEMENTATION)

#define MFP_ArrayLength(arr) (sizeof(arr) / sizeof(arr[0]))

#include <stdio.h>
API void MFP__End(MFP_Platform *platform) {
    if (platform->graphicsEnd) {
        platform->graphicsEnd(platform);
    }
    // reset input
    MFP_Input *input = &platform->input;
    input->textLength = 0;
    input->text[0] = 0;
    input->downKeysBuffer[0] = 0;
    input->downKeysBufferSize = 0;
    input->mouseLeft.pressed = false;
    input->mouseWheelDelta = 0.0f;


    // update time
    MFP_Timer *timer = &platform->timer;
    unsigned long int ticks = MFP__GetTicks();

    // TODO: deltaSec returns inf if target fps is not set????
#ifdef _WIN32
    timer->deltaSec = ((float) ticks - (float) timer->ticks) / (float) frequency.QuadPart;
#else
    timer->deltaSec = (float) (ticks - timer->ticks) / 1000.0f;
#endif
    timer->timeSec += timer->deltaSec;
    timer->ticks = ticks;
    float targetFrameTime = 1.0f / platform->targetFps;
    if (timer->deltaSec < targetFrameTime) {
        float remaining = targetFrameTime - timer->deltaSec;
        MFP_SleepMs(1000 * remaining);
        timer->deltaSec = targetFrameTime;
    }
    timer->fps = (1.0f / timer->deltaSec);
}


// {{{ Linux
#ifdef __linux__

typedef struct MFP_Linux {
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
    int depth;

    void *graphicHandle;
} MFP_Linux;

API MFP_Linux* MFP__GetLinux(MFP_Platform *platform) {
    return (MFP_Linux *) platform->os; 
}

API void MFP__LinuxDispatchKey(MFP_Input *input, MFP_ButtonState *state, bool down) {
    state->pressed = !state->down && down;
    state->released = state->down && !down;
    state->down = down;
}

API void MFP__LinuxDispatchXKey(MFP_Input *input, XKeyEvent *event, bool down) {
    KeySym sym = XLookupKeysym(event, 0);
    if (sym >= XK_space && sym <= XK_asciitilde) {
        MFP_ButtonState *state = &input->keys[sym];
        MFP__LinuxDispatchKey(input, state, down);
        if (state->pressed) {
            // handle text input
            char buffer[16] = {};
            size_t amount = 0;
            KeySym key;
            amount = XLookupString(event, buffer, sizeof(buffer), &key, 0);
            memcpy(&input->text[input->textLength], buffer, amount);
            input->textLength += amount;
            assert(input->textLength < 256);
            input->text[input->textLength] = 0;
        } else if (state->down) {
            input->downKeysBuffer[input->downKeysBufferSize++] = sym;
        }
    } else {
        char mysym = 0;
        switch (sym) {
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
        if (mysym != 0) {
            MFP_ButtonState *state = &input->keys[(int)mysym];
            MFP__LinuxDispatchKey(input, state, down);
        }
    }
}

API unsigned long int MFP__GetTicks() {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC_RAW, &now);
    unsigned long int res = ((unsigned long int) now.tv_sec * 1000) + ((unsigned long int) now.tv_nsec / 1000000);
    return res;
}

API void MFP_Init(MFP_Platform *platform) {
    platform->os = malloc(sizeof(MFP_Linux));
    MFP_Linux *plat = (MFP_Linux *) platform->os;
    plat->display = XOpenDisplay(NULL);
    plat->screen = DefaultScreen(plat->display);
    plat->root = RootWindow(plat->display, plat->screen);

    plat->depth = XDefaultDepth(plat->display, plat->screen);
    plat->visual = XDefaultVisual(plat->display, plat->screen);
    plat->colormap = XDefaultColormap(plat->display, plat->screen);
    // XWarpPointer(plat->display, None, plat->root, 0, 0, 0, 0, 0, 0);
    XUngrabPointer(plat->display, CurrentTime);
    XFlush(plat->display);
}

API void MFP_SetTargetFps(MFP_Platform *platform, int fps) {
    platform->targetFps = fps;
}

API void MFP_Begin(MFP_Platform *platform) {
    if (platform->timer.ticks == 0) {
        platform->timer.ticks = MFP__GetTicks();
    }
    XEvent event;
    MFP_Input *input = &platform->input;

    for (size_t i = 0; i < MFP_ArrayLength(input->keys); ++i) {
        MFP_ButtonState *state = &input->keys[i];
        state->pressed = false;
        state->released = false;
    }

    MFP_Linux* oslinux = MFP__GetLinux(platform);

    while(XPending(oslinux->display)) {
        XNextEvent(oslinux->display, &event);
        switch (event.type) {
            case KeyPress: {
                MFP__LinuxDispatchXKey(input, &event.xkey, true);
            } break;
            case KeyRelease: {
                // NOTE: xserver sends release and press if key is hold down
                // this will ignore those events
                if (XEventsQueued(oslinux->display, QueuedAfterReading))
                {
                    XEvent next;
                    XPeekEvent(oslinux->display, &next);
                    if (next.type == KeyPress && next.xkey.time == event.xkey.time)
                    {
                        if (input->enableKeyRepeat)
                            MFP__LinuxDispatchXKey(input, &event.xkey, false);
                        else
                            continue;
                    }
                } 
                MFP__LinuxDispatchXKey(input, &event.xkey, false);
            } break;
            case ConfigureNotify: {
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
            case MotionNotify: {
                XMotionEvent xme = event.xmotion;
                input->mouseDx = xme.x - input->mouseX;
                input->mouseX = xme.x;
                input->mouseDy = (platform->window.height - xme.y) - input->mouseY;
                input->mouseY = platform->window.height - xme.y;
            } break;
            case ButtonPress: {
                //XButtonEvent xbe = event.xbutton;
                input->mouseLeft.down = true;
            } break;
            case ButtonRelease: {
                input->mouseLeft.released = true;
                input->mouseLeft.pressed = input->mouseLeft.down;
                input->mouseLeft.down = false;
            } break;
        }
    }
}

API void MFP_End(MFP_Platform *platform, bool swapBuffers) {
    MFP__End(platform);
}

API void MFP_WindowOpen(MFP_Platform *platform, const char *title, int x, int y, int width, int height) {
    assert(!platform->window.isOpen);
    MFP_Linux *oslinux = MFP__GetLinux(platform);
    int screen = XDefaultScreen(oslinux->display);
    if (x == MFP_WINDOWPOS_CENTER) {
        int displayWidth = XDisplayWidth(oslinux->display, screen);
        platform->window.x = (displayWidth - width) * 0.5f;
    } else {
        platform->window.x = x;
    }
    if (y == MFP_WINDOWPOS_CENTER) {
        int displayHeight = XDisplayHeight(oslinux->display, screen);
        platform->window.y = (displayHeight - height) * 0.5f;
    } else {
        platform->window.y = y;
    }
    platform->window.width = width;
    platform->window.height = height;
    platform->window.title = title;
    XSetWindowAttributes windowAttributes;
    windowAttributes.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
        StructureNotifyMask | PointerMotionMask | EnterWindowMask | LeaveWindowMask |
        ButtonPressMask | ButtonReleaseMask;

    windowAttributes.colormap = oslinux->colormap;

    // TODO: multi monitor setup: x coordinate is always relative root
    // window so the window will launch on the left monitor not the fouces one
    oslinux->window = XCreateWindow(oslinux->display,
                                    oslinux->root,
                                    platform->window.x,
                                    platform->window.y,
                                    platform->window.width,
                                    platform->window.height,
                                    0,
                                    oslinux->depth,
                                    InputOutput,
                                    oslinux->visual,
                                    CWColormap | CWEventMask,
                                    &windowAttributes);


    XMapWindow(oslinux->display, oslinux->window);
    if (platform->window.title)
        XStoreName(oslinux->display, oslinux->window, platform->window.title); 

    if (platform->graphicsAfterWindow) {
        platform->graphicsAfterWindow(platform);
    }
    platform->window.isOpen = true;
}

API void MFP_WindowClose(MFP_Platform *platform) {
    MFP_Window *window = &platform->window;
    MFP_Linux *oslinux = MFP__GetLinux(platform);
    XDestroyWindow(oslinux->display, oslinux->window);
    XCloseDisplay(oslinux->display);
    window->isOpen = false;
}

API bool MFP_WindowShouldClose(MFP_Platform *platform) {
    return !platform->window.isOpen;
}

API void MFP_WindowToggleFullscreen(MFP_Platform *platform) {
    MFP_Linux *oslinux = MFP__GetLinux(platform);
    XEvent e;
    e.xclient.type = ClientMessage;
    e.xclient.window = oslinux->window;
    e.xclient.message_type = XInternAtom(oslinux->display, "_NET_WM_STATE", true);
    e.xclient.format = 32;
    e.xclient.data.l[0] = 2;
    e.xclient.data.l[1] = XInternAtom(oslinux->display, "_NET_WM_STATE_FULLSCREEN", true);
    e.xclient.data.l[2] = 0;
    e.xclient.data.l[3] = 1;
    e.xclient.data.l[4] = 0;
    XSendEvent(oslinux->display, oslinux->root, False, SubstructureRedirectMask | SubstructureNotifyMask, &e);
}

API bool MFP_IsKeyPressed(MFP_Platform *platform, char c) {
    return platform->input.keys[c].pressed;
}

API bool MFP_IsKeyDown(MFP_Platform *platform, char c) {
    return platform->input.keys[c].down;
}

API bool MFP_IsKeyReleased(MFP_Platform *platform, char c) {
    return platform->input.keys[c].released;
}

API float MFP_GetDeltaSec(MFP_Platform *platform) {
    return platform->timer.deltaSec;
}

API double MFP_GetTimeSec(MFP_Platform *platform) {
    return platform->timer.timeSec;
}

API void MFP_SleepMs(long ms) {
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms / 1000) * 1000000;
}

API void MFP_Destroy(MFP_Platform *platform) {
    // TODO:
}

// }}}

// {{{ Windows
#else

static MFP_Platform *g_platform = NULL;
typedef struct MFP_Win32 {
    HWND window;
    HDC dc;

    LARGE_INTEGER frequency;
    void *graphicHandle;
} MFP_Win32;

API unsigned long int MFP__GetTicks() {
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return counter.QuadPart;
}

API MFP_Win32 *MFP_GetWin32(MFP_Platform *platform) {
    return (MFP_Win32 *) platform->os; 
}

API void MFP_Init(MFP_Platform *platform) {
    platform->os = malloc(sizeof(MFP_Win32));
    QueryPerformanceFrequency(&platform->os.frequency);
}

API void MFP_Begin(MFP_Platform *platform) {
    if (platform->timer.ticks == 0) {
        platform->timer.ticks = MFP__GetTicks();
    }
    for (size_t i = 0; i < MFP__AMOUNT_KEYS; ++i) {
        MFP_ButtonState *state = &platform->input.keys[i];
        state->pressed = false;
        state->released = false;
    }

    MFP_Win32 *os = MFP_GetWin32(platform);
    MSG msg;
    while (PeekMessage(&msg, os->window, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (platform->graphicsBegin) {
        platform->graphicsBegin(platform);
    }
}

API void MFP_End(MFP_Platform *platform, bool swapBuffers=true) {
    MFP__End(platform);
}

API void MFP_Destroy(MFP_Platform *platform) {
    MFP_Win32 *os = MFP_GetWin32(platform);
    DestroyWindow(os->window);
    DeleteObject(os->dc);
}

API void MFP_WindowToggleFullscreen(MFP_Platform *platform) {
    void *dummy = platform;
    assert(dummy);
}

API void MFP__GetClientRect(HWND window, int *x, int *y, size_t *width, size_t *height) {
    RECT clientRect;
    GetClientRect(window, &clientRect);

    *x = clientRect.left;
    *y = clientRect.top;
    *width = clientRect.right - clientRect.left;
    *height = clientRect.bottom - clientRect.top;
}

void MFP__Win32SetMousePos(MFP_Platform *platform, LPARAM lParam) {
    int x = LOWORD(lParam);
    int y = platform->window.height - HIWORD(lParam);
    platform->input.mouseX = x;
    platform->input.mouseY = y;
}

void MFP__Win32DispatchKeyToInput(MFP_Input *input, MFP_ButtonState *state, bool down) {
    state->pressed = !state->down && down;
    state->released = state->down && !down;
    if (input->enableKeyRepeat && state->down && down)
    {
        // NOTE: this will generate keyrepeate
        state->pressed = true;
    }
    state->down = down;
}

void MFP__Win32DispatchKey(MFP_Input *input, size_t keycode, bool down) {
    int keyIndex = keycode;
    char buf[1024] = {};
    OutputDebugString(buf);

    // NOTE: only big letter chars
    assert(keyIndex < 'a' || keyIndex > 'z');
    if (keyIndex >= 'A' && keyIndex <= 'Z')
    {
        int upperLowerOffset = 32;
        bool shift_down = input->keys[MF_KEY_SHIFT].down;
        if (shift_down || input->keys[MF_KEY_CTRL].down)
        {
            // NOTE: if upper case also dispatch lower case press
            MFP_ButtonState *state = &input->keys[keyIndex + upperLowerOffset];
            MFP__Win32DispatchKeyToInput(input, state, down);

            input->text[input->textLength++] = shift_down ? keyIndex : keyIndex + 32;
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

    MFP_ButtonState *state = &input->keys[keyIndex];
    if (down) {
        input->downKeysBuffer[input->downKeysBufferSize++] = keyIndex;
    }
    MFP__Win32DispatchKeyToInput(input, state, down);
    return;
}

LRESULT CALLBACK MFP__Win32Proc(HWND wnd, UINT message, WPARAM wParam, LPARAM lParam) {
    LRESULT res = 0;
    //Platform *platform = (Platform *) GetWindowLongPtr(wnd, GWLP_USERDATA);
    assert(g_platform != NULL);
    MFP_Win32 *os = MFP_GetWin32(g_platform);
    switch (message) {
        case WM_SIZE: {
            if (g_platform->callback) {
                MFP_Window *window = &g_platform->window;
                MFP__GetClientRect(os->window,
                                   &window->x,
                                   &window->y,
                                   &window->width,
                                   &window->height);
                g_platform->callback(MFP_MESSAGE_WINDOW_SIZE);
            }
            break;
        }
        case WM_LBUTTONDOWN: {
            MFP__Win32SetMousePos(g_platform, lParam);
            g_platform->input.mouseLeft.pressed = true;
            break;
        }
        case WM_MOUSEMOVE: {
            MFP__Win32SetMousePos(g_platform, lParam);
        }
        case WM_MOUSEWHEEL: {
            int delta = GET_WHEEL_DELTA_WPARAM(wParam);
            g_platform->input.mouseWheelDelta = (float) delta;
            break;
        }
        case WM_CLOSE:
        case WM_DESTROY:
        case WM_QUIT: {
            g_platform->window.isOpen = false;
            break;
        }
        case WM_SYSKEYDOWN: {
            printf("WM_SYSKEYDOWN %d\n", (size_t) wParam);
            break;
        }
        case WM_KEYDOWN: {
            // TODO: if the _WinProc loop is slow the amount of repeats will be more than one
            // so the application misses some key repeats but maybe this is okay
            //size_t amount = lParam & 0x0FFFF;
            MFP__Win32DispatchKey(&g_platform->input, wParam, true);
            break;
        }
        case WM_SYSKEYUP: {
            break;
        }
        case WM_KEYUP: {
            MFP__Win32DispatchKey(&g_platform->input, wParam, false);
            break;
        }
        case WM_EXITSIZEMOVE: {
            int x, y;
            size_t width, height;
            mfp__get_client_rect(os->window, &x, &y, &width, &height);
            MFP_Window *window = &g_platform->window;
            if (x != window->x ||
                y != window->y ||
                width != window->width ||
                height != window->height) {
                window->x = x;
                window->y = y;
                window->width = width;
                window->height = height;
            }
        }
        case WM_PAINT: {
            //PAINTSTRUCT Paint;
            //HDC dc = BeginPaint(wnd, &Paint);
            //EndPaint(wnd, &Paint);
            res = DefWindowProcA(wnd, message, wParam, lParam);
            break;
        }
        case WM_CHAR: {
            WCHAR utfChar = (WCHAR) wParam;
            int shifted = ((int) lParam >> 16) & 0x0F;
            char asciiChar;
            int length = WideCharToMultiByte(CP_ACP, 0, &utfChar, 1, &asciiChar, 1, 0, 0);
            MFP_Input *input = &g_platform->input;
            if (length == 1 && asciiChar >= ' ')
            {
                if (asciiChar == '\r') {
                    asciiChar = '\n';
                }
                input->text[input->textLength++] = asciiChar;
                input->text[input->textLength] = 0;
                //res = 1;
            }
        }

        default:
            res = DefWindowProcA(wnd, message, wParam, lParam);
    }
    return res;
}

API void MFP_WindowOpen(MFP_Platform *platform, const char *title, int x, int y, int width, int height) {
    WNDCLASS wc = {};

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = MFP__Win32Proc;
    wc.lpszClassName = "Platform";
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    RegisterClass(&wc);

    // TODO: this is crap
    g_platform = platform;


    MFP_Win32 *os = MFP_GetWin32(platform);
    MFP_Window *window = &platform->window;
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
    int pixelFormatIndex = ChoosePixelFormat(os->dc, &desiredPixelFormat);
    PIXELFORMATDESCRIPTOR suggestedPixelFormat;
    DescribePixelFormat(os->dc, pixelFormatIndex, sizeof(suggestedPixelFormat), &suggestedPixelFormat);
    SetPixelFormat(os->dc, pixelFormatIndex, &suggestedPixelFormat);

    // TODO: client rect

    mfp__get_client_rect(os->window, &window->x, &window->y, &window->width, &window->height);
    window->isOpen = true;
}

API void MFP_WindowClose(MFP_Platform *platform) {
    assert(platform);
}

#endif
// }}}

#endif // MF_PLATFORM_IMPLEMENTATION
#endif // MF_PLATFORM_H

