#include <EGL/egl.h>
#include "loadgl.h"

void loadgl(void) {
	loadgl_loader((loadgl_fn) eglGetProcAddress);
}
