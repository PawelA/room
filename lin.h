#define PI 3.1415926535f

typedef float vec3[3];
typedef float mat4[4][4];

void v3_assign(vec3 r, vec3 a);
void v3_add(vec3 r, vec3 a);
void v3_sub(vec3 r, vec3 a);
void v3_mul(vec3 r, float s);

float v3_dot(vec3 u, vec3 v);
void v3_cross(vec3 r, vec3 u, vec3 v);
void v3_rot_perp(vec3 u1, vec3 u2, vec3 u3, float a);
void v3_normalize(vec3 u);

void v3_proj(vec3 r, vec3 a);
void v3_rej(vec3 r, vec3 a);

void v3_interp(vec3 r, vec3 u1, vec3 u2, vec3 u3, vec3 c);

void m4_set_identity(mat4 m);
void m4_set_perspective(mat4 m, float theta, float near, float far);
void m4_translate(mat4 m, float x, float y, float z);
void m4_rotate(mat4 m, float theta, float x, float y, float z);
void m4_scale(mat4 m, float x, float y, float z);

void m4_assign(mat4 r, mat4 a);
void m4_mul(mat4 r, mat4 a, mat4 b);

#define v3_set(v, a0, a1, a2) \
	v[0] = a0; \
	v[1] = a1; \
	v[2] = a2

#define m4_set(m, \
		a11, a12, a13, a14, \
		a21, a22, a23, a24, \
		a31, a32, a33, a34, \
		a41, a42, a43, a44) \
	m[0][0] = a11; m[0][1] = a12; m[0][2] = a13; m[0][3] = a14; \
	m[1][0] = a21; m[1][1] = a22; m[1][2] = a23; m[1][3] = a24; \
	m[2][0] = a31; m[2][1] = a32; m[2][2] = a33; m[2][3] = a34; \
	m[3][0] = a41; m[3][1] = a42; m[3][2] = a43; m[3][3] = a44

