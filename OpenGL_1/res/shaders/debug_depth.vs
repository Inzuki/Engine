
#version 330 core

layout (location = 0) in vec3 pos_c;
layout (location = 1) in vec2 tex_c;

out vec2 tex_out;

void main(){
    tex_out = tex_c;
    gl_Position = vec4(pos_c, 1.0);
}