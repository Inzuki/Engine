
#version 330 core

out vec4 fragColor;

in vec2 tex_out;

uniform sampler2D depth_map;
uniform float near_plane;
uniform float far_plane;

void main(){             
    fragColor = vec4(vec3(texture(depth_map, tex_out).r), 1.0);
}