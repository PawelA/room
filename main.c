#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "gl.h"
#include "loadgl.h"
#include "shader.h"
#include "window.h"
#include "keys.h"
#include "lin.h"
#include "fps.h"
#include "model.h"

SHADER_EXTERN(phong);
unsigned u_model_mat;
unsigned u_view_mat;
unsigned u_persp_mat;
unsigned u_light_mat;
unsigned u_view_coords;
unsigned u_ambient_color;
unsigned u_diffuse_color;
unsigned u_specular_color;
unsigned u_shininess;
unsigned u_shadow;

SHADER_EXTERN(depth);
unsigned u_depth_model_mat;
unsigned u_depth_view_persp_mat;

vec3 pos = { 0, 1, 0 };
float rot_v, rot_h;

int pressed[256];
struct window_evt input;

void handle_input(void) {
	while (window_get_evt(&input)) {
		switch (input.type) {
		case WINDOW_KEY_UP:
			pressed[input.key] = 0;
			break;
		case WINDOW_KEY_DOWN:
			pressed[input.key] = 1;
			if (input.key == KEY_Q) exit(0);
			if (input.key == KEY_ESCAPE) {
				input.focused = 0;
				window_show_cursor();
				window_untrap_cursor();
			}
			break;
		case WINDOW_BUTTON_DOWN:
			if (input.button == BUTTON_LEFT) {
				input.focused = 1;
				window_hide_cursor();
				window_trap_cursor();
			}
		case WINDOW_FOCUS:
			if (input.focused) {
				window_hide_cursor();
				window_trap_cursor();
			}
			else {
				window_show_cursor();
				window_untrap_cursor();
			}
			break;
		case WINDOW_CLOSE:
			exit(0);
		}
	}

	{
		float s = sinf(rot_h);
		float c = cosf(rot_h);
		float fwd = 0.03f * (pressed[KEY_W] - pressed[KEY_S]);
		float lft = 0.03f * (pressed[KEY_A] - pressed[KEY_D]);
		float up  = 0.03f * (pressed[KEY_SPACE] - pressed[KEY_LSHIFT]);
		pos[0] += -fwd * s - lft * c;
		pos[1] += +up;
		pos[2] += -fwd * c + lft * s;
	}

	if (input.focused) {
		int dx = input.x - input.width / 2;
		int dy = input.y - input.height / 2;
		rot_h -= 0.005 * dx;
		rot_v -= 0.005 * dy;
		if (rot_v < -PI / 2) rot_v = -PI / 2;
		if (rot_v > +PI / 2) rot_v = +PI / 2;
		window_move_cursor(input.width / 2, input.height / 2);
	}
}

void phong_render_model(mat4 model_mat, struct model *mod) {
	glUniformMatrix4fv(u_model_mat, 1, 1, &model_mat[0][0]);
	glUniform3fv(u_ambient_color, 1, mod->mtl.ambient);
	glUniform3fv(u_diffuse_color, 1, mod->mtl.diffuse);
	glUniform3fv(u_specular_color, 1, mod->mtl.specular);
	glUniform1f(u_shininess, mod->mtl.shininess);

	glBindVertexArray(mod->vert_array_id);
	glDrawElements(GL_TRIANGLES, mod->face_cnt * 3, GL_UNSIGNED_INT, 0);
}

void depth_render_model(mat4 model_mat, struct model *mod) {
	glUniformMatrix4fv(u_depth_model_mat, 1, 1, &model_mat[0][0]);
	glBindVertexArray(mod->vert_array_id);
	glDrawElements(GL_TRIANGLES, mod->face_cnt * 3, GL_UNSIGNED_INT, 0);
}

struct shadow_map {
	unsigned framebuffer_id;
	unsigned texture_id;
	mat4 view_persp_mat;
	int width, height;
};

void shadow_map_make(struct shadow_map *map, float x, float y, float z, float lh, float lv, int w, int h) {
	unsigned framebuffer;
	unsigned tex;

	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	map->framebuffer_id = framebuffer;
	map->texture_id = tex;
	m4_set_identity(map->view_persp_mat);
	m4_translate(map->view_persp_mat, -x, -y, -z);
	m4_rotate(map->view_persp_mat, -lh, 0, 1, 0);
	m4_rotate(map->view_persp_mat, -lv, 1, 0, 0);
	{
		mat4 persp;
		m4_set_perspective(persp, PI/2, 0.1, 50);
		m4_mul(map->view_persp_mat, persp, map->view_persp_mat);
	}
}

int main(void) {
	mat4 view_mat;
	mat4 persp_mat;
	mat4 room_model_mat;
	mat4 lamp_model_mat;
	mat4 bulb_model_mat;
	mat4 chair_model_mat;
	unsigned phong_shader_id;
	unsigned depth_shader_id;
	unsigned width = 1920, height = 1080;

	struct model room, lamp, bulb, chair;
	struct shadow_map sh_map;
	
	if (!window_init()) exit(1);
	if (!window_create(width, height, WINDOW_FULLSCREEN)) exit(1);

	loadgl();
	glClearColor(1, 1, 1, 1);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	phong_shader_id = SHADER_COMPILE(phong);
	u_model_mat = SHADER_UNIFORM(u_model_mat);
	u_view_mat = SHADER_UNIFORM(u_view_mat);
	u_persp_mat = SHADER_UNIFORM(u_persp_mat);
	u_light_mat = SHADER_UNIFORM(u_light_mat);
	u_view_coords = SHADER_UNIFORM(u_view_coords);
	u_ambient_color = SHADER_UNIFORM(u_ambient_color);
	u_diffuse_color = SHADER_UNIFORM(u_diffuse_color);
	u_specular_color = SHADER_UNIFORM(u_specular_color);
	u_shininess = SHADER_UNIFORM(u_shininess);
	u_shadow = SHADER_UNIFORM(u_shadow);

	depth_shader_id = SHADER_COMPILE(depth);
	u_depth_model_mat = SHADER_UNIFORM(u_model_mat);
	u_depth_view_persp_mat = SHADER_UNIFORM(u_view_persp_mat);

	window_map();
	model_load(&room, "models/room");
	model_load(&lamp, "models/lamp");
	model_load(&bulb, "models/bulb");
	model_load(&chair, "models/chair");
	shadow_map_make(&sh_map, 1, 1, 0.5, -2.0, 0.7, 200, 200);

	m4_set_perspective(persp_mat, PI / 2, 0.1, 50);
	m4_set_identity(room_model_mat);
	m4_set_identity(lamp_model_mat);
	m4_scale(lamp_model_mat, 0.35, 0.35, 0.35);
	m4_translate(lamp_model_mat, 1, 2.5, 0.5);
	m4_set_identity(bulb_model_mat);
	m4_scale(bulb_model_mat, 0.12, 0.12, 0.12);
	m4_translate(bulb_model_mat, 1, 2, 0.5);
	m4_set_identity(chair_model_mat);
	m4_rotate(chair_model_mat, -2.1, 0, 1, 0);
	m4_scale(chair_model_mat, 0.4, 0.4, 0.4);
	m4_translate(chair_model_mat, -1.2, 0.8, -1);

	fps_limit(60);

	while (1) {
		glUseProgram(depth_shader_id);
		glViewport(0, 0, sh_map.width, sh_map.height);
		glBindFramebuffer(GL_FRAMEBUFFER, sh_map.framebuffer_id);
		glClear(GL_DEPTH_BUFFER_BIT);

		glUniformMatrix4fv(u_depth_view_persp_mat, 1, 1, &sh_map.view_persp_mat[0][0]);
		depth_render_model(room_model_mat, &room);
		depth_render_model(lamp_model_mat, &lamp);
		depth_render_model(bulb_model_mat, &bulb);
		depth_render_model(chair_model_mat, &chair);

		glUseProgram(phong_shader_id);
		glViewport(0, 0, width, height);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, sh_map.texture_id);
		m4_set_identity(view_mat);
		m4_translate(view_mat, -pos[0], -pos[1], -pos[2]);
		m4_rotate(view_mat, -rot_h, 0, 1, 0);
		m4_rotate(view_mat, -rot_v, 1, 0, 0);
		m4_scale(view_mat, (float) height / width, 1, 1);
		glUniformMatrix4fv(u_view_mat, 1, 1, &view_mat[0][0]);
		glUniformMatrix4fv(u_persp_mat, 1, 1, &persp_mat[0][0]);
		glUniformMatrix4fv(u_light_mat, 1, 1, &sh_map.view_persp_mat[0][0]);
		glUniform3fv(u_view_coords, 1, pos);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		phong_render_model(room_model_mat, &room);
		phong_render_model(lamp_model_mat, &lamp);
		phong_render_model(bulb_model_mat, &bulb);
		phong_render_model(chair_model_mat, &chair);
		
		window_swap_buffers();
		fps_frame();
		handle_input();
	}
}
