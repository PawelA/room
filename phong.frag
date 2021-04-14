#version 330 core

in vec3 frag_coords;
in vec3 view_coords;
in vec3 normal;

in vec3 ambient_color;
in vec3 diffuse_color;
in vec3 specular_color;
in float shininess;

out vec3 out_color;

uniform sampler2D u_shadow;
uniform mat4 u_light_mat;

vec3 light_coords = vec3(1, 2, 0.5);

void main() {
	vec3 n = normalize(normal);
	vec3 l = normalize(light_coords - frag_coords);
	vec3 v = normalize(view_coords - frag_coords);
	vec3 r = reflect(-l, n);
	float ambient = 1;
	float diffuse = max(0, dot(n, l));
	float specular = pow(max(0, dot(v, r)), shininess);
    //out_color = vec3(ambient + diffuse + specular);
    out_color =
    	ambient * ambient_color +
    	diffuse * diffuse_color +
    	specular * specular_color;

    vec4 frag = u_light_mat * vec4(frag_coords, 1.0);
    vec3 proj = frag.xyz / frag.w;
    proj = proj * 0.5 + 0.5;
    float depth = texture(u_shadow, proj.xy).r;
    out_color *= 1 + 0.0000001 * depth;
}
