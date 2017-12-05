#version 330 core

out vec4 fragColor;

uniform vec3 light_color;

/*****************\
|* main function *|
\*****************/
void main(){
	fragColor = vec4(light_color, 1.0);
} 