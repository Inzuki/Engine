
#version 330 core
#include "lighting.glh"

in vec2 tex_out;
in vec3 norm_out;
in vec3 worldPos;
in vec4 shadow_coord;

out vec4 fragColor;

uniform sampler2D diffuse;
uniform sampler2D shadow_map;

uniform DirectionalLight directional_light;

float calc_shadow(){
	vec3 proj_coords = (shadow_coord.xyz / shadow_coord.w) * vec3(0.5) + vec3(0.5);
	return step(proj_coords.z, texture(shadow_map, proj_coords.xy).r);
}

/*****************\
|* main function *|
\*****************/
void main(){
	vec4 lighting = calc_directional_light(
						directional_light,
						normalize(norm_out),
						worldPos
					) *
					calc_shadow();
					
	fragColor = lighting * texture(diffuse, tex_out.xy);
}