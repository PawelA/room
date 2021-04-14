#include <GL/glx.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include "window.h"
#include "keys.h"

#define GLX_CONTEXT_MAJOR_VERSION_ARB       0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB       0x2092
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display *, GLXFBConfig, GLXContext, Bool, const int *);

static int visattrs[] = { 
	GLX_RGBA,
	GLX_RED_SIZE, 1,
	GLX_GREEN_SIZE, 1,
	GLX_BLUE_SIZE, 1,
	GLX_DOUBLEBUFFER,
	GLX_DEPTH_SIZE, 24,
	None
};

static int ctxattrs[] = {
	GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
    GLX_CONTEXT_MINOR_VERSION_ARB, 3,
    None
};

static int keys[256] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, KEY_ESCAPE, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I,
	KEY_O, KEY_P, 0, 0, 0, 0, KEY_A, KEY_S,
	KEY_D, KEY_F, KEY_G, KEY_H, KEY_J, KEY_K, KEY_L, 0,
	0, 0, KEY_LSHIFT, 0, KEY_Z, KEY_X, KEY_C, KEY_V,
	KEY_B, KEY_N, KEY_M, 0, 0, 0, 0, 0,
	0, KEY_SPACE, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, KEY_UP,
	0, KEY_LEFT, KEY_RIGHT, 0, KEY_DOWN, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
};

static Display *dpy;
static Window win;
static int screen;

static Atom atom_delete;
static Atom atom_state;
static Atom atom_fullscreen;
static Atom atom_type;
static Atom atom_dialog;
static Cursor null_cursor;
static glXCreateContextAttribsARBProc glXCreateContextAttribsARB;

static void make_null_cursor(void) {
	Pixmap pix;
	XColor black = { 0 };
	char empty = 0;
	
	pix = XCreateBitmapFromData(dpy, win, &empty, 1, 1);
	null_cursor = XCreatePixmapCursor(dpy, pix, pix, &black, &black, 0, 0);
	XFreePixmap(dpy, pix);
}

int window_init(void) {
	dpy = XOpenDisplay(NULL);
	if (!dpy)
		return 0;
	atom_delete     = XInternAtom(dpy, "WM_DELETE_WINDOW", 0);
	atom_state      = XInternAtom(dpy, "_NET_WM_STATE", 0);
	atom_fullscreen = XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", 0);
	atom_type       = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE", 0);
	atom_dialog     = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_DIALOG", 0);
	glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)
		glXGetProcAddressARB((const unsigned char *) "glXCreateContextAttribsARB");
	return 1;
}

int window_create(int width, int height, int mode) {
	Window root;
	XVisualInfo *visinfo;
	XSetWindowAttributes winattrs;
	GLXContext ctx;
	GLXFBConfig *fbcfg;
	int fbcfg_cnt;
	int screen;

	screen = XDefaultScreen(dpy);
	root = XRootWindow(dpy, screen);
	fbcfg = glXChooseFBConfig(dpy, screen, visattrs, &fbcfg_cnt);
	if (!fbcfg)
		return 0;
	ctx = glXCreateContextAttribsARB(dpy, fbcfg[0], NULL, 1, ctxattrs);

	visinfo = glXGetVisualFromFBConfig(dpy, fbcfg[0]);
	winattrs.colormap = XCreateColormap(dpy, root, visinfo->visual, AllocNone);
	winattrs.event_mask =
			  ExposureMask
			| KeyPressMask
			| KeyReleaseMask
			| ButtonPressMask
			| ButtonReleaseMask
			| FocusChangeMask
			| PointerMotionMask
			| StructureNotifyMask;

	win = XCreateWindow(dpy, root, 0, 0, width, height, 0, visinfo->depth,
			InputOutput, visinfo->visual, CWColormap | CWEventMask, &winattrs);

	XSetWMProtocols(dpy, win, &atom_delete, 1);
	switch (mode) {
	case WINDOW_FULLSCREEN:
		XChangeProperty(dpy, win, atom_state, XA_ATOM, 32, PropModeReplace,
				(unsigned char *) &atom_fullscreen, 1);
		break;
	case WINDOW_FLOATING:
		XChangeProperty(dpy, win, atom_type, XA_ATOM, 32, PropModeReplace,
				(unsigned char *) &atom_dialog, 1);
		break;

	}

	glXMakeCurrent(dpy, win, ctx);
	
	make_null_cursor();
	return 1;
}

void window_map(void) {
	XMapWindow(dpy, win);
}

void window_destroy(void) {
	XDestroyWindow(dpy, win);
}

void window_show_cursor(void) {
	XDefineCursor(dpy, win, 0);
}

void window_hide_cursor(void) {
	XDefineCursor(dpy, win, null_cursor);
}

void window_trap_cursor(void) {
	XGrabPointer(dpy, win, 1,
		ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
		GrabModeAsync, GrabModeAsync, win, None, CurrentTime);
}

void window_untrap_cursor(void) {
	XUngrabPointer(dpy, CurrentTime);
}

void window_move_cursor(int x, int y) {
	XWarpPointer(dpy, None, win, 0, 0, 0, 0, x, y);
	XFlush(dpy);
}

void window_swap_buffers(void) {
	glXSwapBuffers(dpy, win);
}

int window_get_evt(struct window_evt *evt) {
	XEvent xev;
	int key;
	
	while (XPending(dpy) > 0) {
		XNextEvent(dpy, &xev);
		switch (xev.type) {
		case KeyPress:
			if (!(key = keys[xev.xkey.keycode & 0xff]))
				break;
			evt->key = key;
			evt->type = WINDOW_KEY_DOWN;
			return 1;
		case KeyRelease:
			if (!(key = keys[xev.xkey.keycode & 0xff]))
				break;
			evt->key = key;
			evt->type = WINDOW_KEY_UP;
			return 1;
		case ButtonPress:
			evt->button = xev.xbutton.button;
			evt->type = WINDOW_BUTTON_DOWN;
			return 1;
		case ButtonRelease:
			evt->button = xev.xbutton.button;
			evt->type = WINDOW_BUTTON_UP;
			return 1;
		case MotionNotify:
			evt->x = xev.xmotion.x;
			evt->y = xev.xmotion.y;
			evt->type = WINDOW_MOUSE_MOVE;
			return 1;
		case FocusIn:
			evt->focused = 1;
			evt->type = WINDOW_FOCUS;
			return 1;
		case FocusOut:
			evt->focused = 0;
			evt->type = WINDOW_FOCUS;
			return 1;
		case ConfigureNotify:
			evt->width  = xev.xconfigure.width;
			evt->height = xev.xconfigure.height;
			evt->type = WINDOW_RESIZE;
			return 1;
		case ClientMessage:
			evt->type = WINDOW_CLOSE;
			return 1;
		}
	}

	return 0;
}
