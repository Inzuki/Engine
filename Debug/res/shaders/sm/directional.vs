
#version 330 core

layout (location = 0) in vec3 pos_c;
layout (location = 1) in vec2 tex_c;
layout (location = 2) in vec3 norm_c;

out vec2 tex_out;
out vec3 norm_out;
out vec3 worldPos;
out vec4 light_out;

uniform mat4 projection_mat;
uniform mat4 view_mat;
uniform mat4 transform;

uniform mat4 light_projection;
uniform mat4 light_view;

void main(){
    norm_out = (transform * vec4(norm_c, 0.0)).xyz;
	worldPos = (transform * vec4(pos_c, 1.0)).xyz;
    tex_out = vec2(tex_c.x, 1.0 - tex_c.y);
    light_out = light_projection * light_view * vec4(worldPos, 1.0);
    gl_Position = projection_mat * view_mat * transform * vec4(pos_c, 1.0);
}