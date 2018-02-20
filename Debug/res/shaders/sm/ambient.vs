
#version 330 core

layout (location = 0) in vec3 pos_c;
layout (location = 1) in vec2 tex_c;

out vec2 tex_out;

uniform mat4 transform;
uniform mat4 view_mat;
uniform mat4 projection_mat;

void main(){
    gl_Position = projection_mat * view_mat * transform * vec4(pos_c, 1.0);
	tex_out = vec2(tex_c.x, 1.0 - tex_c.y);
}