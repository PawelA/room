#version 330 core

layout(location = 0) in vec3 a_coords;

uniform mat4 u_model_mat;
uniform mat4 u_view_persp_mat;

void main() {
	gl_Position = u_view_persp_mat * u_model_mat * vec4(a_coords, 1.0);
}
