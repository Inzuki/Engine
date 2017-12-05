#version 330 core

in vec2 tex_out;
in vec3 norm_out;
in vec3 worldPos;

out vec4 fragColor;

struct BaseLight {
	vec3 color;
	float intensity;
};

struct Attenuation {
	float constant;
	float linear;
	float exponent;
};

struct PointLight {
	BaseLight base;
	Attenuation attenuation;
	vec3 position;
	float range;
};

uniform vec3 eyePos;

uniform sampler2D diffuse;

uniform float specular_intensity;
uniform float specular_power;

uniform PointLight point_light;

vec4 calc_light(BaseLight base, vec3 direction, vec3 normal){
	float diffuse_factor = dot(normal, -direction);
	vec4 diffuse_color = vec4(0.0, 0.0, 0.0, 0.0);
	vec4 specular_color = vec4(0.0, 0.0, 0.0, 0.0);
	
	if(diffuse_factor > 0.0){
		diffuse_color = vec4(base.color, 1.0) * base.intensity * diffuse_factor;
		
		// specular calculation
		vec3 direction_to_eye = normalize(eyePos - worldPos);
		vec3 reflect_direction = normalize(reflect(direction, normal));
		float specular_factor = dot(direction_to_eye, reflect_direction);
		specular_factor = pow(specular_factor, specular_power);
		if(specular_factor > 0.0){
			specular_color = vec4(base.color, 1.0) * specular_intensity * specular_factor;
		}
	}
	
	return diffuse_color + specular_color;
}

vec4 calc_point_light(PointLight point_light, vec3 normal){
	vec3 light_direction = worldPos - point_light.position;
	float distance_to_point = length(light_direction);
	
	if(distance_to_point > point_light.range)
		return vec4(0.0, 0.0, 0.0, 0.0);
	
	light_direction = normalize(light_direction);
	
	vec4 color = calc_light(point_light.base, light_direction, normal);
	
	float attenuation =
		point_light.attenuation.constant + 
		point_light.attenuation.linear * distance_to_point + 
		point_light.attenuation.exponent * distance_to_point * distance_to_point +
		0.0001;
		// adding the 0.0001 prevents division to zero
	
	return color / attenuation;
}

/*****************\
|* main function *|
\*****************/
void main(){
	fragColor = texture(diffuse, tex_out.xy) * calc_point_light(point_light, normalize(norm_out));
}