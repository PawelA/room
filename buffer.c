#include <stdio.h>
#include <stdlib.h>
#include "gl.h"

struct contents {
	char *buff;
	int size;
};

static int load_contents(char *name, struct contents *cont) {
	FILE *fp;
	char *buff = NULL;
	int size;
	int res;

	fp = fopen(name, "rb");
	if (!fp) {
		perror(name);
		return 0;
	}

	res = fseek(fp, 0, SEEK_END);
	if (res == -1) {
		perror("fseek");
		goto cleanup;
	}
	
	size = ftell(fp);
	if (size == -1) {
		perror("ftell");
		goto cleanup;
	}

	buff = malloc(size);
	if (!buff) {
		perror("malloc");
		goto cleanup;
	}

	res = fseek(fp, 0, SEEK_SET);
	if (res == -1) {
		perror("fseek");
		goto cleanup;
	}

	res = fread(buff, 1, size, fp);
	if (res != size) {
		printf("fread: short read\n");
		goto cleanup;
	}

	cont->buff = buff;
	cont->size = size;

	fclose(fp);
	return 1;

cleanup:
	fclose(fp);
	free(buff);
	return 0;
}

GLuint buffer_load_vert_data(char *name, int *size) {
	struct contents cont;
	GLuint buffer;

	if (!load_contents(name, &cont))
		return -1;

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, cont.size, cont.buff, GL_STATIC_DRAW);
	if (size) *size = cont.size;

	free(cont.buff);
	return buffer;
}

GLuint buffer_load_elem_data(char *name, int *size) {
	struct contents cont;
	GLuint buffer;

	if (!load_contents(name, &cont))
		return -1;

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cont.size, cont.buff, GL_STATIC_DRAW);
	if (size) *size = cont.size;

	free(cont.buff);
	return buffer;
}

void *buffer_read(char *name, int size, int *nmemb) {
	int header[2];
	char *buff;
	FILE *fp;

	fp = fopen(name, "r");
	fread(header, sizeof(int), 2, fp);
	buff = malloc(header[0] * header[1]);
	fread(buff, header[0], header[1], fp);
	fclose(fp);

	if (nmemb) *nmemb = header[1];
	return buff;
}
