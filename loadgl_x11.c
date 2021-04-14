#include <GL/glx.h>
#include "loadgl.h"

void loadgl(void) {
	loadgl_loader((loadgl_fn) glXGetProcAddressARB);
}
