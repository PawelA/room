#include "os.h"

static long long last;
static long long frame;

void fps_limit(int fps) {
	frame = 1000000000 / fps;
	last = time_get();
}

void fps_frame(void) {
	long long now;

	now = time_get();
	if (now >= last && now < last + frame) {
		time_wait(last + frame - now);
		last += frame;
	}
	else {
		last = now;
	}
}
