#include <stdio.h>
#include <stdlib.h>
#include "gl.h"

static char *last_shader_name;
static GLuint last_shader;

static int shader_log_len(GLuint shader) {
	int len;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
	return len;
}

static int program_log_len(GLuint program) {
	int len;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
	return len;
}

static void print_shader_log(GLuint shader, int len) {
	char *log;
	if ((log = malloc(len))) {
		glGetShaderInfoLog(shader, len, &len, log);
		fwrite(log, 1, len, stdout);
		free(log);
	}
}

static void print_program_log(GLuint program, int len) {
	char *log;
	if ((log = malloc(len))) {
		glGetProgramInfoLog(program, len, &len, log);
		fwrite(log, 1, len, stdout);
		free(log);
	}
}

static int compile_status(GLuint shader) {
	int status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	return status;
}

static int link_status(GLuint prog) {
	int status;
	glGetProgramiv(prog, GL_LINK_STATUS, &status);
	return status;
}

static int check_vert(GLuint vert, char *name) {
	int len = shader_log_len(vert);
	if (len > 0) {
		printf("%s.vert:\n", name);
		print_shader_log(vert, len);
	}
	return compile_status(vert);
}

static int check_frag(GLuint frag, char *name) {
	int len = shader_log_len(frag);
	if (len > 0) {
		printf("%s.frag:\n", name);
		print_shader_log(frag, len);
	}
	return compile_status(frag);
}

static int check_prog(GLuint prog, char *name) {
	int len = program_log_len(prog);
	if (len > 0) {
		printf("%s:\n", name);
		print_program_log(prog, len);
	}
	return link_status(prog);
}

GLuint shader_compile(char *name,
		const char *vert_code, int vert_size,
		const char *frag_code, int frag_size) {
	GLuint prog;
	GLuint vert;
	GLuint frag;

	last_shader_name = name;
	last_shader = -1;

	vert = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert, 1, &vert_code, &vert_size);
	glCompileShader(vert);
	if (!check_vert(vert, name)) exit(1);

	frag = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag, 1, &frag_code, &frag_size);
	glCompileShader(frag);
	if (!check_frag(frag, name)) exit(1);

	prog = glCreateProgram();
	glAttachShader(prog, vert);
	glAttachShader(prog, frag);
	glLinkProgram(prog);
	if (!check_prog(prog, name)) exit(1);

	glDetachShader(prog, vert);
	glDeleteShader(vert);

	glDetachShader(prog, frag);
	glDeleteShader(frag);

	last_shader = prog;
	return prog;
}

GLuint shader_uniform(char *name) {
	GLint location = glGetUniformLocation(last_shader, name);
	if (location == -1) {
		printf("%s:\nuniform `%s' not active\n", last_shader_name, name);
		exit(1);
	}
	return location;
}
