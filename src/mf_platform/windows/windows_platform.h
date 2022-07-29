struct Platform;
static Platform* g_platform = NULL;

LRESULT CALLBACK mfp__window_proc(HWND wnd, UINT message, WPARAM wParam, LPARAM lParam);
struct Platform: IPlatform {

    HWND wnd;
    HDC dc;
    HGLRC hgl;

    void init();
    void destroy();
    void window_create(const char *title, i32 x, i32 y, i32 w, i32 h);
    void window_close();
    void begin();
    void Platform::end(bool swapBuffers);

    u64 _get_ticks();
    void _get_client_rect();
    void _set_mouse_pos(LPARAM lParam);
    void _dispatch_key_to_input(ButtonState *state, bool down);
    void _dispatch_key(u32 keycode, bool down);
};

void Platform::init() {
    g_platform = this;
}

void Platform::destroy() {
    wglDeleteContext(this->hgl);
    DestroyWindow(this->wnd);
    DeleteObject(this->dc);
}

void Platform::window_create(const char *title, i32 x, i32 y, i32 w, i32 h) {
    WNDCLASS wc = {};
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = mfp__window_proc;
    wc.lpszClassName = title;
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    RegisterClass(&wc);

    this->window.x = x;
    this->window.y = y;
    this->window.title = title;

    RECT sizeToRequest = {};
    sizeToRequest.left = 0;
    sizeToRequest.right = w;
    sizeToRequest.top = 0;
    sizeToRequest.bottom = h;
    DWORD style = (WS_OVERLAPPEDWINDOW | WS_VISIBLE) & ~WS_THICKFRAME;
    AdjustWindowRect(&sizeToRequest, style, false);
    this->wnd = CreateWindowA(wc.lpszClassName,
                               this->window.title,
                               style,
                               this->window.x, this->window.y,
                               sizeToRequest.right - sizeToRequest.left,
                               sizeToRequest.bottom - sizeToRequest.top,
                               0, 0, 0, 0);
    this->dc = GetDC(this->wnd);

    PIXELFORMATDESCRIPTOR desiredPixelFormat = {};
    desiredPixelFormat.nSize = sizeof(desiredPixelFormat);
    desiredPixelFormat.nVersion = 1;
    desiredPixelFormat.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
    desiredPixelFormat.cColorBits = 32;
    desiredPixelFormat.cAlphaBits = 8;
    desiredPixelFormat.iLayerType = PFD_MAIN_PLANE;

    // TODO: pixelformat always needed??
    i32 pixelFormatIndex = ChoosePixelFormat(this->dc, &desiredPixelFormat);
    PIXELFORMATDESCRIPTOR suggestedPixelFormat;
    DescribePixelFormat(this->dc, pixelFormatIndex, sizeof(suggestedPixelFormat), &suggestedPixelFormat);
    SetPixelFormat(this->dc, pixelFormatIndex, &suggestedPixelFormat);

    this->_get_client_rect();
    this->window.isOpen = true;
}

void Platform::window_close() {
}


void Platform::begin() {
    if (this->timer.ticks == 0) {
        this->timer.ticks = _get_ticks();
    }

    for (size_t i = 0; i < MFP__AMOUNT_KEYS; ++i) {
        ButtonState *state = &this->input.keys[i];
        state->pressed = false;
        state->released = false;
    }

    MSG msg;
    while (PeekMessage(&msg, this->wnd, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}
    
void Platform::end(bool swapBuffers) {
    // reset input
    this->input.textLength = 0;
    this->input.text[0] = 0;
    this->input.downKeysBuffer[0] = 0;
    this->input.downKeysBufferSize = 0;
    this->input.mouseLeft.pressed = false;
    this->input.mouseWheelDelta = 0.0f;

    static LARGE_INTEGER frequency;
    if (frequency.QuadPart == 0) {
        QueryPerformanceFrequency(&frequency);
    }

    Timer *timer = &this->timer;
    u64 ticks = _get_ticks();

    timer->deltaSec = ((float) ticks - (float) timer->ticks) / (float) frequency.QuadPart;
    timer->ticks = ticks;
    timer->fps = (1.0f / timer->deltaSec);

    if (swapBuffers) {
        SwapBuffers(dc);
    }
}

u64 Platform::_get_ticks() {
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return counter.QuadPart;
}

void Platform::_get_client_rect() {
    RECT clientRect;
    GetClientRect(this->wnd, &clientRect);

    this->window.x = clientRect.left;
    this->window.y = clientRect.top;
    this->window.width = clientRect.right - clientRect.left;
    this->window.height = clientRect.bottom - clientRect.top;
}

void Platform::_set_mouse_pos(LPARAM lParam) {
    i32 x = LOWORD(lParam);
    i32 y = this->window.height - HIWORD(lParam);
    this->input.mouseX = x;
    this->input.mouseY = y;
}

void Platform::_dispatch_key_to_input(ButtonState *state, bool down) {
    state->pressed = !state->down && down;
    state->released = state->down && !down;
    if (this->input.enableKeyRepeat && state->down && down) {
        // NOTE: this will generate keyrepeate
        state->pressed = true;
    }
    state->down = down;
}

void Platform::_dispatch_key(u32 keycode, bool down) {
    i32 keyIndex = keycode;
    char buf[1024] = {};
    OutputDebugString(buf);

    // NOTE: only big letter chars
    MF_Assert(keyIndex < 'a' || keyIndex > 'z');
    if (keyIndex >= 'A' && keyIndex <= 'Z')
    {
        i32 upperLowerOffset = 32;
        bool shift_down = this->input.keys[MF_KEY_SHIFT].down;
        if (shift_down || this->input.keys[MF_KEY_CTRL].down) {
            // NOTE: if upper case also dispatch lower case press
            ButtonState *state = &this->input.keys[keyIndex + upperLowerOffset];
            this->_dispatch_key_to_input(state, down);

            this->input.text[this->input.textLength++] = shift_down ? keyIndex : keyIndex + 32;
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

    ButtonState *state = &this->input.keys[keyIndex];
    if (down) {
        this->input.downKeysBuffer[this->input.downKeysBufferSize++] = keyIndex;
    }
    this->_dispatch_key_to_input(state, down);
    return;
}



LRESULT CALLBACK mfp__window_proc(HWND wnd, UINT message, WPARAM wParam, LPARAM lParam) {
    LRESULT res = 0;
// TODO pass platform
#if 1
    //Platform *platform = (Platform *) GetWindowLongPtr(wnd, GWLP_USERDATA);
    switch (message) {
        case WM_SIZE: {
            if (g_platform->callback) {
                g_platform->_get_client_rect();
                g_platform->callback(MFP_MESSAGE_WINDOW_SIZE);
            }
            break;
        }
        case WM_LBUTTONDOWN: {
            g_platform->_set_mouse_pos(lParam);
            g_platform->input.mouseLeft.pressed = true;
            break;
        }
        case WM_MOUSEMOVE: {
            g_platform->_set_mouse_pos(lParam);
        }
        case WM_MOUSEWHEEL: {
            //i32 keys = GET_KEYSTATE_WPARAM(wParam);
            i32 delta = GET_WHEEL_DELTA_WPARAM(wParam);
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
            printf("WM_SYSKEYDOWN %d\n", (u32) wParam);
            break;
        }
        case WM_KEYDOWN: {
            // TODO: if the _WinProc loop is slow the amount of repeats will be more than one
            // so the application misses some key repeats but maybe this is okay
            //u32 amount = lParam & 0x0FFFF;
            printf("WM_KEYDOWN %d %c\n", (u32) wParam, (char) wParam);
            g_platform->_dispatch_key((u32) wParam, true);
            break;
        }
        case WM_SYSKEYUP: {
            //printf("WM_SYSKEYUP %d\n", (u32) wParam);
            break;
        }
        case WM_KEYUP: {
            g_platform->_dispatch_key((u32) wParam, false);
            break;
        }
        case WM_EXITSIZEMOVE: {
            break;
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
            i32 length = WideCharToMultiByte(CP_ACP, 0, &utfChar, 1, &asciiChar, 1, 0, 0);
            if (length == 1 && asciiChar >= ' ')
            {
                if (asciiChar == '\r') {
                    asciiChar = '\n';
                }
                g_platform->input.text[g_platform->input.textLength++] = asciiChar;
                g_platform->input.text[g_platform->input.textLength] = 0;
            }
        }

        default:
            res = DefWindowProcA(wnd, message, wParam, lParam);
    }
#endif
    return res;
}

