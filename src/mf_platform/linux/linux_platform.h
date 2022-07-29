#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

typedef Window X11Window;

namespace mf { namespace platform {

struct OsLinux {
    Display *display;
    int screen;
    X11Window root;
    X11Window x11_window;
    //// NOTE visual is also a member of XVisualInfo in this case both members have to be set
    //// a application with OpenGl uses XVisualInfo and a xlib application just uses visual
    //// TODO: just use XVisualInfo
    XVisualInfo *vi;
    Visual *visual;
    Colormap colormap;
    i32 depth;

    // TODO: is there a better way?
    void *graphicHandle;
};

struct Platform: IPlatform {
    OsLinux os;

    void init(); 
    void destroy() {};

    virtual void window_create(const char *title, i32 x, i32 y, i32 w, i32 h);
    virtual void window_close();

    virtual void begin();
    virtual void end();

    u64 _get_ticks();
    void _dispatch_xkey(XKeyEvent *event, bool down);
    void _dispatch_key(ButtonState *state, bool down);
};

void Platform::init() {
    this->os.display = XOpenDisplay(NULL);
    this->os.screen = DefaultScreen(this->os.display);
    this->os.root = RootWindow(this->os.display, this->os.screen);

    this->os.depth = XDefaultDepth(this->os.display, this->os.screen);
    this->os.visual = XDefaultVisual(this->os.display, this->os.screen);
    this->os.colormap = XDefaultColormap(this->os.display, this->os.screen);
}

void Platform::window_create(const char *title, i32 x, i32 y, i32 w, i32 h) {
    this->window.x = x;
    this->window.y = y;
    this->window.width = w;
    this->window.height = h;
    this->window.title = title;
    XSetWindowAttributes windowAttributes;
    windowAttributes.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
        StructureNotifyMask | PointerMotionMask | EnterWindowMask | LeaveWindowMask |
        ButtonPressMask | ButtonReleaseMask;

    windowAttributes.colormap = this->os.colormap;

    // TODO: multi monitor setup: x coordinate is always relative root
    // window so the window will launch on the left monitor not the fouces one
    this->os.x11_window = XCreateWindow(this->os.display,
                                     this->os.root,
                                     this->window.x,
                                     this->window.y,
                                     this->window.width,
                                     this->window.height,
                                     0,
                                     this->os.depth,
                                     InputOutput,
                                     this->os.visual,
                                     CWColormap | CWEventMask,
                                     &windowAttributes);


    XMapWindow(this->os.display, this->os.x11_window);
    if (this->window.title)
        XStoreName(this->os.display, this->os.x11_window, this->window.title); 

    this->window.isOpen = true;
}

void Platform::window_close() {
    XDestroyWindow(this->os.display, this->os.x11_window);
    XCloseDisplay(this->os.display);
    this->window.isOpen = false;
}

void Platform::begin() {
    if (this->timer.ticks == 0) {
        this->timer.ticks = this->_get_ticks();
    }
    XEvent event;

    for (u32 i = 0; i < MF_ArrayLength(this->input.keys); ++i) {
        ButtonState *state = &this->input.keys[i];
        state->pressed = false;
        state->released = false;
    }

    while(XPending(this->os.display)) {
        XNextEvent(this->os.display, &event);
        switch (event.type) {
            case KeyPress: {
                this->_dispatch_xkey(&event.xkey, true);
            } break;
            case KeyRelease: {
                // NOTE: xserver sends release and press if key is hold down
                // this will ignore those events
                if (XEventsQueued(this->os.display, QueuedAfterReading)) {
                    XEvent next;
                    XPeekEvent(this->os.display, &next);
                    if (next.type == KeyPress && next.xkey.time == event.xkey.time) {
                        if (this->input.enableKeyRepeat)
                            this->_dispatch_xkey(&event.xkey, false);
                        else
                            continue;
                    }
                } 
                this->_dispatch_xkey(&event.xkey, false);
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
                this->input.mouseX = xme.x;
                this->input.mouseY = this->window.height - xme.y;
            } break;
            case ButtonPress: {
                //XButtonEvent xbe = event.xbutton;
                this->input.mouseLeft.down = true;
            } break;
            case ButtonRelease: {
                this->input.mouseLeft.released = true;
                this->input.mouseLeft.pressed = this->input.mouseLeft.down;
                this->input.mouseLeft.down = false;
            } break;
        }
    }
}

void Platform::end() {
    u64 ticks = _get_ticks();
    this->timer.deltaSec = (float) (ticks - this->timer.ticks) / 1000.0f;
    this->timer.ticks = ticks;
    this->timer.fps = (1.0f / this->timer.deltaSec);

    if (this->custom_end) {
        this->custom_end(this);
    }
}

u64 Platform::_get_ticks() {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC_RAW, &now);
    u64 res = ((u64) now.tv_sec * 1000) + ((u64) now.tv_nsec / 1000000);
    return res;
}

void Platform::_dispatch_xkey(XKeyEvent *event, bool down) {
    KeySym sym = XLookupKeysym(event, 0);
    if (sym >= XK_space && sym <= XK_asciitilde) {
        ButtonState *state = &this->input.keys[sym];
        this->_dispatch_key(state, down);
        if (state->pressed) {
            // handle text input
            char buffer[16] = {};
            u32 amount = 0;
            KeySym key;
            amount = XLookupString(event, buffer, sizeof(buffer), &key, 0);
            memcpy(&this->input.text[this->input.textLength], buffer, amount);
            this->input.textLength += amount;
            MF_Assert(this->input.textLength < 256);
            this->input.text[this->input.textLength] = 0;
        } else if (state->down) {
            this->input.downKeysBuffer[this->input.downKeysBufferSize++] = sym;
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
        } if (mysym != 0) {
            ButtonState *state = &this->input.keys[(i32)mysym];
            this->_dispatch_key(state, down);
        }
    }
}

void Platform::_dispatch_key(ButtonState *state, bool down) {
    state->pressed = !state->down && down;
    state->released = state->down && !down;
    state->down = down;
}
#if 0

u64 mfp__get_time_micro()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    u64 cycles = ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
    return cycles;
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

#endif

}}
