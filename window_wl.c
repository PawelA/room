#include <wayland-client.h>
#include <wayland-egl.h>
#include <EGL/egl.h>
#include <string.h>
#include "window.h"

static int attrs[] = {
	EGL_RED_SIZE, 8,
	EGL_GREEN_SIZE, 8,
	EGL_BLUE_SIZE, 8,
	EGL_NONE
}; 

static struct wl_display *display;
static struct wl_compositor *compositor;
static struct wl_shell *shell;
static struct wl_surface *surface;
static struct wl_shell_surface *shell_surface;
static struct wl_egl_window *egl_window;

static EGLDisplay egl_display;
static EGLContext egl_ctx;
static EGLSurface egl_surface;


static void registry_add_object(void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version) {
	if (strcmp(interface, "wl_compositor") == 0) {
		compositor = wl_registry_bind (registry, name, &wl_compositor_interface, 1);
	}
	else if (strcmp(interface, "wl_shell") == 0) {
		shell = wl_registry_bind (registry, name, &wl_shell_interface, 1);
	}
}

static void registry_remove_object(void *data, struct wl_registry *registry, uint32_t name) { }

static struct wl_registry_listener registry_listener = { registry_add_object, registry_remove_object };

static void shell_surface_ping (void *data, struct wl_shell_surface *shell_surface, uint32_t serial) {
	wl_shell_surface_pong (shell_surface, serial);
}

static void shell_surface_configure(void *data, struct wl_shell_surface *shell_surface, uint32_t edges, int32_t width, int32_t height) {
	wl_egl_window_resize(egl_window, width, height, 0, 0);
}

static void shell_surface_popup_done(void *data, struct wl_shell_surface *shell_surface) { }

static struct wl_shell_surface_listener shell_surface_listener = {
	shell_surface_ping,
	shell_surface_configure,
	shell_surface_popup_done
};


int window_init(void) {
	struct wl_registry *registry;
	
	display = wl_display_connect(NULL);
	registry = wl_display_get_registry(display);
	wl_registry_add_listener(registry, &registry_listener, NULL);
	wl_display_roundtrip(display);
	
	egl_display = eglGetDisplay(display);
	eglInitialize(egl_display, NULL, NULL);
	
	return 1;
}

int window_create(int width, int height, int mode) {
	EGLConfig egl_cfg;
	int cfg_cnt;
	
	eglBindAPI(EGL_OPENGL_API);
	eglChooseConfig(egl_display, attrs, &egl_cfg, 1, &cfg_cnt);
	egl_ctx = eglCreateContext(egl_display, egl_cfg, EGL_NO_CONTEXT, NULL);
	
	surface = wl_compositor_create_surface(compositor);
	shell_surface = wl_shell_get_shell_surface(shell, surface);
	wl_shell_surface_add_listener(shell_surface, &shell_surface_listener, NULL);
	wl_shell_surface_set_toplevel(shell_surface);
	egl_window = wl_egl_window_create(surface, width, height);
	egl_surface = eglCreateWindowSurface(egl_display, egl_cfg, egl_window, NULL);
	eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_ctx);
	
	return 1;
}

void window_show(void) {
	
}

void window_destroy(void) {
	eglDestroySurface(egl_display, egl_surface);
	wl_egl_window_destroy(egl_window);
	wl_shell_surface_destroy(shell_surface);
	wl_surface_destroy(surface);
	eglDestroyContext(egl_display, egl_ctx);
}

void window_show_cursor(void) {
	
}

void window_hide_cursor(void) {
	
}

void window_set_cursor_pos(int x, int y) {
	
}

void window_swap_buffers(void) {
	eglSwapBuffers(egl_display, egl_surface);
}

int window_get_evt(struct window_evt *evt) {
	wl_display_dispatch_pending(display);
	return 0;
}
