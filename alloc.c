#include <stdio.h>
#include <stdlib.h>

void *alloc(int size) {
	fflush(stdout);
	void *ptr = malloc(size);
	if (!ptr) {
		fprintf(stderr, "error: failed to allocate %d bytes\n", size);
		exit(1);
	}
	return ptr;
}

/*
void *alloc_re(void *ptr, int size) {
	printf("%d\n", size);
	void *ptr_re = realloc(ptr, size);
	if (!ptr_re) {
		fprintf(stderr, "error: failed to allocate %d bytes\n", size);
		exit(1);
	}
	return ptr_re;
}
*/
