
#version 330 core

in vec2 tex_out;

uniform vec3 ambientIntensity;
uniform sampler2D tex;

out vec4 fragColor;

void main(){
	fragColor = texture(tex, tex_out.xy) * vec4(ambientIntensity, 1.0);
}