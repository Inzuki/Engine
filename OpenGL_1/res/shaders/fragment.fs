#version 330 core

const int MAX_POINT_LIGHTS = 4;
const int MAX_SPOT_LIGHTS = 4;

in vec2 tex_out;
in vec3 norm_out;
in vec3 worldPos;

out vec4 fragColor;

struct BaseLight {
	vec3 color;
	float intensity;
};

struct DirectionalLight {
	BaseLight base;
	vec3 direction;
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

struct SpotLight {
	PointLight point_light;
	vec3 direction;
	float cut_off;
};

uniform vec3 eyePos;

uniform vec3 base_color;
uniform vec3 ambient_light;
uniform sampler2D tex;

uniform float specular_intensity;
uniform float specular_power;

uniform DirectionalLight directional_light;
uniform PointLight point_lights[MAX_POINT_LIGHTS];
uniform SpotLight spot_lights[MAX_SPOT_LIGHTS];

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

vec4 calc_directional_light(DirectionalLight directional_light, vec3 normal){
	return calc_light(directional_light.base, -directional_light.direction, normal);
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
		point_light.attenuation.exponent * distance_to_point * distance_to_point + 0.0001;
		// adding the 0.0001 prevents division to zero
	
	return color / attenuation;
}

vec4 calc_spot_light(SpotLight spot_light, vec3 normal){
	vec3 light_direction = normalize(worldPos - spot_light.point_light.position);
	float spot_factor = dot(light_direction, spot_light.direction);
	
	vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
	
	if(spot_factor > spot_light.cut_off)
		color = calc_point_light(spot_light.point_light, normal) * 
				(1.0 - (1.0 - spot_factor) / (1.0 - spot_light.cut_off));

	return color;
}

/*****************\
|* main function *|
\*****************/
void main(){
	vec4 total_light = vec4(ambient_light, 1.0);
	vec4 color = vec4(base_color, 1.0);
	vec4 tex_color = texture(tex, tex_out.xy);
	
	if(tex_color != vec4(0.0, 0.0, 0.0, 0.0))
		color *= tex_color;
		
	vec3 normal = normalize(norm_out);
	
	total_light += calc_directional_light(directional_light, normal);
	
	for(int i = 0; i < MAX_POINT_LIGHTS; i++)
		// this check helps ensure we don't apply the result even if
		// the light doesn't exist,it's a waste of processing power
		if(point_lights[i].base.intensity > 0.0)
			total_light += calc_point_light(point_lights[i], normal);
			
	for(int i = 0; i < MAX_SPOT_LIGHTS; i++)
		// this check helps ensure we don't apply the result even if
		// the light doesn't exist,it's a waste of processing power
		if(spot_lights[i].point_light.base.intensity > 0.0)
			total_light += calc_spot_light(spot_lights[i], normal);
	
	fragColor = color * total_light;
} 