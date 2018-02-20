
#version 330 core
#include "lighting.glh"

in vec2 tex_out;
in vec3 norm_out;
in vec3 worldPos;

out vec4 fragColor;

uniform sampler2D diffuse;

uniform SpotLight spot_light;

/*****************\
|* main function *|
\*****************/
void main(){
	fragColor = texture(diffuse, tex_out.xy) * calc_spot_light(spot_light, normalize(norm_out), worldPos);
}