
#version 330 core
#include "lighting.glh"

in vec2 tex_out;
in vec3 norm_out;
in vec3 worldPos;

out vec4 fragColor;

uniform sampler2D diffuse;

uniform PointLight point_light;

/*****************\
|* main function *|
\*****************/
void main(){
	fragColor = texture(diffuse, tex_out.xy) * calc_point_light(point_light, normalize(norm_out), worldPos);
}