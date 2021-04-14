#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "loadgl.h"

static void *load(const char *name) {
	void *addr;
	HMODULE module;

	addr = (void *) wglGetProcAddress(name);
	if (!addr) {
		module = LoadLibrary("opengl32.dll");
		addr = (void *) GetProcAddress(module, name);
	}

	return addr;
}

void loadgl(void) {
	loadgl_loader(load);
}
