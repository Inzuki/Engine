
#version 330 core

in vec2 tex_out;

uniform vec3 ambientIntensity;
uniform sampler2D tex;

out vec4 fragColor;

void main(){
	vec3 color = texture(tex, tex_out).rgb;
	vec3 ambient = ambientIntensity * color;
	fragColor = vec4(ambient * color, 1.0);
}