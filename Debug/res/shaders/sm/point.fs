
#version 330 core

in vec2 tex_out;
in vec3 norm_out;
in vec3 worldPos;

out vec4 FragColor;

uniform sampler2D diffuse;
uniform samplerCube shadow_map;

uniform float far_plane;
uniform vec3 light_pos;
uniform vec3 light_color;
uniform vec3 eyePos;

float ShadowCalculation(){
	vec3 frag_to_light = worldPos - light_pos;
	
	float closest_depth = texture(shadow_map, frag_to_light).r;
	closest_depth *= far_plane;
	//float closest_depth = texture(shadow_map, frag_to_light).r;
	//closest_depth *= 0.5 + 0.5;
	float current_depth = length(frag_to_light);
	
	float bias = 0.05;
	float shadow = current_depth - bias > closest_depth ? 1.0 : 0.0; 
	//FragColor = vec4(vec3(current_depth / far_plane), 1.0);
	
	return shadow;
}

void main(){
    vec3 color = texture(diffuse, tex_out).rgb;
    vec3 normal = normalize(norm_out);
	// ambient
    vec3 ambient = 0.3 * color;
    // diffuse
    vec3 lightDir = normalize(light_pos - worldPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse_var = diff * light_color;
    // specular
    vec3 viewDir = normalize(eyePos - worldPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * light_color; 
    // calculate shadow
    float shadow = ShadowCalculation();                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse_var + specular)) * color;    
    
    FragColor = vec4(lighting, 1.0);
}