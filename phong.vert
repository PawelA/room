#version 330 core

layout(location = 0) in vec3 a_coords;
layout(location = 1) in vec3 a_normal;

out vec3 frag_coords;
out vec3 view_coords;
out vec3 normal;

out vec3 ambient_color;
out vec3 diffuse_color;
out vec3 specular_color;
out float shininess;

uniform mat4 u_model_mat;
uniform mat4 u_view_mat;
uniform mat4 u_persp_mat;
uniform vec3 u_view_coords;

uniform vec3 u_ambient_color;
uniform vec3 u_diffuse_color;
uniform vec3 u_specular_color;
uniform float u_shininess;

void main() {
	gl_Position = u_persp_mat * u_view_mat * u_model_mat * vec4(a_coords, 1.0);

	frag_coords = vec3(u_model_mat * vec4(a_coords, 1.0));
	view_coords = u_view_coords;
	normal = mat3(u_model_mat) * a_normal;

	ambient_color = u_ambient_color; 
	diffuse_color = u_diffuse_color; 
	specular_color = u_specular_color; 
	shininess = u_shininess; 
}
