#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

#include "window.h"
#include "keys.h"

#define WINDOW_CLASS "winclass"
#define WINDOW_TITLE "_"

static HWND hwnd;
static HDC hdc;
static HGLRC hrc;

static int keys[256] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I,
	KEY_O, KEY_P, 0, 0, 0, 0, KEY_A, KEY_S,
	KEY_D, KEY_F, KEY_G, KEY_H, KEY_J, KEY_K, KEY_L, 0,
	0, 0, 0, 0, KEY_Z, KEY_X, KEY_C, KEY_V,
	KEY_B, KEY_N, KEY_M, 0, 0, 0, 0, 0,
};

static LRESULT CALLBACK win_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

int window_init(void) {
	WNDCLASS wc;
	
    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = win_proc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = GetModuleHandle(NULL);
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = WINDOW_CLASS;
	
	return RegisterClass(&wc) ? 0 : -1;
}

int window_create(int width, int height, int mode) {
	int pf;
	PIXELFORMATDESCRIPTOR pfd;

    hwnd = CreateWindow(WINDOW_CLASS, WINDOW_TITLE, WS_OVERLAPPEDWINDOW,
            0, 0, width, height, NULL, NULL, GetModuleHandle(NULL), NULL);
    if (!hwnd)
		return -1;

	hdc = GetDC(hwnd);

	memset(&pfd, 0, sizeof(pfd));
	pfd.nSize      = sizeof(pfd);
	pfd.nVersion   = 1;
	pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;

	pf = ChoosePixelFormat(hdc, &pfd);
	if (!pf)
		return -1;

	if (!SetPixelFormat(hdc, pf, &pfd))
		return -1;

	DescribePixelFormat(hdc, pf, sizeof(pfd), &pfd);
	ReleaseDC(hwnd, hdc);

	hdc = GetDC(hwnd);
	hrc = wglCreateContext(hdc);
	wglMakeCurrent(hdc, hrc);

	return 0;
}

void window_show(void) {
    ShowWindow(hwnd, SW_SHOW);
}

void window_destroy(void) {
	wglMakeCurrent(NULL, NULL);
	ReleaseDC(hwnd, hdc);
    wglDeleteContext(hrc);
    DestroyWindow(hwnd);
}


void window_show_cursor(void) {
	ShowCursor(1);
}

void window_hide_cursor(void) {
	ShowCursor(0);
}

void window_get_cursor_pos(int *x, int *y) {
	POINT pt;
	GetCursorPos(&pt);
	*x = pt.x;
	*y = pt.y;
}


void window_swap_buffers(void) {
	SwapBuffers(hdc);
}


static LRESULT CALLBACK win_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    struct window_evt *evt;
    int key;
    
    evt = (struct window_evt *) GetWindowLongPtr(hwnd, GWLP_USERDATA);
    switch (msg) {
    case WM_KEYUP:
    case WM_KEYDOWN:
        if ((key = keys[HIWORD(lparam)])) {
            evt->key = key;
            evt->type = (msg == WM_KEYDOWN ? WINDOW_KEY_PRESS : WINDOW_KEY_RELEASE);
        }
        break;
    case WM_LBUTTONUP:
        evt->button = BUTTON_LEFT;
        evt->type = WINDOW_BUTTON_RELEASE;
        break;
    case WM_LBUTTONDOWN:
        evt->button = BUTTON_LEFT;
        evt->type = WINDOW_BUTTON_PRESS;
        break;
    case WM_MOUSEMOVE:
        evt->x = LOWORD(lparam);
        evt->y = HIWORD(lparam);
        evt->type = WINDOW_MOVE;
        break;
    case WM_CLOSE:
        evt->type = WINDOW_CLOSE;
        break;
    default:
        return DefWindowProc(hwnd, msg, wparam, lparam);
    }
    return 0;
}

int window_get_evt(struct window_evt *evt) {
	MSG msg;
    
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) evt);
    evt->type = 0;
	while (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE)) {
        DispatchMessage(&msg);
        if (evt->type)
            return 1;
    }
    return 0;
}

