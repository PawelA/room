#include <string.h>
#include <math.h>
#include "lin.h"

void v3_assign(vec3 r, vec3 a) {
	r[0] = a[0];
	r[1] = a[1];
	r[2] = a[2];
}

void v3_add(vec3 r, vec3 a) {
	r[0] += a[0];
	r[1] += a[1];
	r[2] += a[2];
}

void v3_sub(vec3 r, vec3 a) {
	r[0] -= a[0];
	r[1] -= a[1];
	r[2] -= a[2];
}

void v3_mul(vec3 r, float s) {
	r[0] *= s;
	r[1] *= s;
	r[2] *= s;
}

float v3_dot(vec3 u, vec3 v) {
	return u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
}

void v3_cross(vec3 r, vec3 u, vec3 v) {
	r[0] = u[1] * v[2] - u[2] * v[1];
	r[1] = u[2] * v[0] - u[0] * v[2];
	r[2] = u[0] * v[1] - u[1] * v[0];
}

void v3_normalize(vec3 u) {
	float s = 1 / sqrtf(u[0] * u[0] + u[1] * u[1] + u[2] * u[2]);
	u[0] *= s;
	u[1] *= s;
	u[2] *= s;
}

void m4_set_identity(mat4 m) {
	m4_set(m,
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);
}

void m4_set_perspective(mat4 m, float theta, float n, float f) {
	float t = 1 / tanf(theta / 2);
	float r = 1 / (n - f);
	m4_set(m,
		t, 0, 0, 0,
		0, t, 0, 0,
		0, 0, r * (n + f), 2 * r * n * f,
		0, 0, -1, 0);
}

void m4_translate(mat4 m, float x, float y, float z) {
	m[0][3] += x;
	m[1][3] += y;
	m[2][3] += z;
}

void m4_rotate(mat4 m, float theta, float x, float y, float z) {
	float c = cos(theta);
	float s = sin(theta);
	int i;
	vec3 k, v, kv;
	float kk;

	k[0] = x; k[1] = y; k[2] = z;
	for (i = 0; i < 4; i++) {
		v[0] = m[0][i]; v[1] = m[1][i]; v[2] = m[2][i];
		v3_cross(kv, k, v);
		kk = v3_dot(k, v) * (1 - c);
		m[0][i] = v[0] * c + kv[0] * s + k[0] * kk;
		m[1][i] = v[1] * c + kv[1] * s + k[1] * kk;
		m[2][i] = v[2] * c + kv[2] * s + k[2] * kk;
	}
}

void m4_scale(mat4 m, float x, float y, float z) {
	int i;
	for (i = 0; i < 4; i++) {
		m[0][i] *= x;
		m[1][i] *= y;
		m[2][i] *= z;
	}
}

void m4_flip(mat4 m, float x, float y, float z) {
	int i;
	float d;

	for (i = 0; i < 4; i++) {
		d = x * m[0][i] + y * m[1][i] + z * m[2][i];
		m[0][i] -= 2 * d * x;
		m[1][i] -= 2 * d * y;
		m[2][i] -= 2 * d * z;
	}
}

void m4_assign(mat4 r, mat4 a) {
	memcpy(r, a, sizeof(mat4));
}

void m4_mul(mat4 r, mat4 a, mat4 b) {
	int i, j, k;
	mat4 s = { 0 };

	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			for (k = 0; k < 4; k++)
				s[i][j] += a[i][k] * b[k][j];
	memcpy(r, s, sizeof(mat4));
}
