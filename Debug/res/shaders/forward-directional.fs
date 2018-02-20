
#version 330 core
#include "lighting.glh"

in vec2 tex_out;
in vec3 norm_out;
in vec3 worldPos;

out vec4 fragColor;

uniform sampler2D diffuse;

uniform DirectionalLight directional_light;

/*****************\
|* main function *|
\*****************/
void main(){
	vec4 lighting = calc_directional_light(
						directional_light,
						normalize(norm_out),
						worldPos
					);
					
	fragColor = lighting * texture(diffuse, tex_out.xy);
}