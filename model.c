#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "model.h"
#include "alloc.h"
#include "gl.h"

enum {
	OBJ_V,
	OBJ_VN,
	OBJ_F,
	OBJ_COMMENT,
	OBJ_EOF,
};

enum {
	MTL_KA,
	MTL_KD,
	MTL_KS,
	MTL_NS,
	MTL_COMMENT,
	MTL_EOF,
};

struct obj {
	int vert_cnt;
	float *verts;

	int normal_cnt;
	float *normals;

	int face_cnt;
	int *vert_inds;
	int *normal_inds;
};

static void obj_skip_line(FILE *fp) {
	char c;
	do {
		c = fgetc(fp);
	} while (c != EOF && c != '\n');
}

static int obj_read_type(FILE *fp) {
	int c;
	do {
		c = fgetc(fp);
	} while (isspace(c));

	if (c == 'v') {
		c = fgetc(fp);
		if (c == ' ') return OBJ_V;
		else if (c == 'n') {
			c = fgetc(fp);
			if (c == ' ') return OBJ_VN;
			goto obj_end;
		}
		goto obj_end;
	}
	else if (c == 'f') {
		c = fgetc(fp);
		if (c == ' ') return OBJ_F;
		goto obj_end;
	}

obj_end:
	if (c == '\n') return -1;
	else if (c == EOF) return OBJ_EOF;
	else return OBJ_COMMENT;
}

static int mtl_read_type(FILE *fp) {
	int c;
	do {
		c = fgetc(fp);
	} while (isspace(c));

	if (c == 'K') {
		c = fgetc(fp);
		if (c == 'a') {
			c = fgetc(fp);
			if (c == ' ') return MTL_KA;
			goto mtl_end;
		}
		else if (c == 'd') {
			c = fgetc(fp);
			if (c == ' ') return MTL_KD;
			goto mtl_end;
		}
		else if (c == 's') {
			c = fgetc(fp);
			if (c == ' ') return MTL_KS;
			goto mtl_end;
		}
		goto mtl_end;
	}
	else if (c == 'N') {
		c = fgetc(fp);
		if (c == 's') {
			c = fgetc(fp);
			if (c == ' ') return MTL_NS;
			goto mtl_end;
		}
		goto mtl_end;
	}

mtl_end:
	if (c == '\n') return -1;
	else if (c == EOF) return MTL_EOF;
	else return MTL_COMMENT;
}

int obj_read(struct obj *obj, char *filename) {
	int type;
	FILE *fp;

	memset(obj, 0, sizeof(*obj));

	fp = fopen(filename, "r");
	if (!fp) return 0;

	while ((type = obj_read_type(fp)) != OBJ_EOF) {
		switch (type) {
		case OBJ_V: obj->vert_cnt++; break;
		case OBJ_VN: obj->normal_cnt++; break;
		case OBJ_F: obj->face_cnt++; break;
		case OBJ_COMMENT: obj_skip_line(fp); break;
		}
	}

	fseek(fp, 0, SEEK_SET);
	obj->verts = alloc(3 * sizeof(float) * obj->vert_cnt);
	obj->normals = alloc(3 * sizeof(float) * obj->normal_cnt);
	obj->vert_inds = alloc(3 * sizeof(int) * obj->face_cnt);
	obj->normal_inds = alloc(3 * sizeof(int) * obj->face_cnt);

	{
		float x, y, z;
		int v1, v2, v3;
		int n1, n2, n3;
		int v_i = 0, vn_i = 0, f_i = 0;
		while ((type = obj_read_type(fp)) != OBJ_EOF) {
			switch (type) {
			case OBJ_V:
				fscanf(fp, "%f %f %f", &x, &y, &z);
				obj->verts[v_i++] = x;
				obj->verts[v_i++] = y;
				obj->verts[v_i++] = z;
				break;
			case OBJ_VN:
				fscanf(fp, "%f %f %f", &x, &y, &z);
				obj->normals[vn_i++] = x;
				obj->normals[vn_i++] = y;
				obj->normals[vn_i++] = z;
				break;
			case OBJ_F:
				fscanf(fp, "%d//%d %d//%d %d//%d", &v1, &n1, &v2, &n2, &v3, &n3);
				obj->vert_inds[f_i] = v1 - 1;
				obj->normal_inds[f_i++] = n1 - 1;
				obj->vert_inds[f_i] = v2 - 1;
				obj->normal_inds[f_i++] = n2 - 1;
				obj->vert_inds[f_i] = v3 - 1;
				obj->normal_inds[f_i++] = n3 - 1;
				break;
			case OBJ_COMMENT:
				obj_skip_line(fp);
				break;
			}
		}
	}

	fclose(fp);
	return 1;
}

void obj_free(struct obj *obj) {
	free(obj->verts);
	free(obj->normals);
	free(obj->vert_inds);
	free(obj->normal_inds);
}

int mtl_read(struct mtl *mtl, char *filename) {
	int type;
	FILE *fp;

	fp = fopen(filename, "r");
	memset(mtl, 0, sizeof(*mtl));

	{
		float x, y, z;
		float s;
		while ((type = mtl_read_type(fp)) != MTL_EOF) {
			switch (type) {
			case MTL_KA:
				fscanf(fp, "%f %f %f", &x, &y, &z);
				mtl->ambient[0] = x;
				mtl->ambient[1] = y;
				mtl->ambient[2] = z;
				break;
			case MTL_KD:
				fscanf(fp, "%f %f %f", &x, &y, &z);
				mtl->diffuse[0] = x;
				mtl->diffuse[1] = y;
				mtl->diffuse[2] = z;
				break;
			case MTL_KS:
				fscanf(fp, "%f %f %f", &x, &y, &z);
				mtl->specular[0] = x;
				mtl->specular[1] = y;
				mtl->specular[2] = z;
				break;
			case MTL_NS:
				fscanf(fp, "%f", &s);
				mtl->shininess = s;
				break;
			}
		}
	}

	fclose(fp);
	return 1;
}

char *cat(char *a, char *b) {
	int len = strlen(a) + strlen(b);
	char *c = alloc(len + 1);
	strcpy(c, a);
	strcat(c, b);
	return c;
}

int model_load(struct model *model, char *base) {
	int i, j;
	int found;
	int vert_cnt = 0;
	float *coords;
	float *normals;
	int *elems;
	char *filename;
	struct obj obj;

	filename = cat(base, ".obj");
	obj_read(&obj, filename);
	free(filename);
	
	filename = cat(base, ".mtl");
	mtl_read(&model->mtl, filename);
	free(filename);

	elems = alloc(3 * sizeof(int) * obj.face_cnt);

	for (i = 0; i < 3 * obj.face_cnt; i++) {
		found = 0;
		for (j = 0; j < i; j++) {
			if (obj.vert_inds[i] == obj.vert_inds[j] && obj.normal_inds[i] == obj.normal_inds[j]) {
				found = 1;
				elems[i] = elems[j];
				break;
			}
		}
		if (!found)
			elems[i] = vert_cnt++;
	}

	coords = alloc(3 * sizeof(float) * vert_cnt);
	normals = alloc(3 * sizeof(float) * vert_cnt);

	for (i = 0; i < 3 * obj.face_cnt; i++) {
		coords[3 * elems[i]] = obj.verts[3 * obj.vert_inds[i]];
		coords[3 * elems[i] + 1] = obj.verts[3 * obj.vert_inds[i] + 1];
		coords[3 * elems[i] + 2] = obj.verts[3 * obj.vert_inds[i] + 2];
		
		normals[3 * elems[i]] = obj.normals[3 * obj.normal_inds[i]];
		normals[3 * elems[i] + 1] = obj.normals[3 * obj.normal_inds[i] + 1];
		normals[3 * elems[i] + 2] = obj.normals[3 * obj.normal_inds[i] + 2];
	}

	model->vert_cnt = vert_cnt;
	model->face_cnt = obj.face_cnt;

	glGenVertexArrays(1, &model->vert_array_id);
	glBindVertexArray(model->vert_array_id);

	glGenBuffers(1, &model->coord_buff_id);
	glBindBuffer(GL_ARRAY_BUFFER, model->coord_buff_id);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float) * vert_cnt, coords, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);

	glGenBuffers(1, &model->normal_buff_id);
	glBindBuffer(GL_ARRAY_BUFFER, model->normal_buff_id);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float) * vert_cnt, normals, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, 0, 0, 0);
	
	glGenBuffers(1, &model->element_buff_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->element_buff_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(int) * obj.face_cnt, elems, GL_STATIC_DRAW);

	free(coords);
	free(normals);
	free(elems);
	obj_free(&obj);

	return 0;
}

