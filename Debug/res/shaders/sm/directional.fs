
#version 330 core

in vec2 tex_out;
in vec3 norm_out;
in vec3 worldPos;
in vec4 light_out;

out vec4 FragColor;

uniform sampler2D diffuse;
uniform sampler2D shadow_map;

uniform vec3 light_pos;
uniform vec3 light_color;
uniform vec3 eyePos;

float ShadowCalculation(vec4 fragPosLightSpace){
    // perform perspective divide and transform to [0,1] range
    vec3 projCoords = (fragPosLightSpace.xyz / fragPosLightSpace.w) * vec3(0.5) + vec3(0.5);
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadow_map, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
	// have a bias to prevent fragments from being considered below the surface
	vec3 normal = normalize(norm_out);
    vec3 lightDir = normalize(light_pos - worldPos);
	float bias = max(0.001 * (1.0 - dot(normal, lightDir)), 0.0001);
    // check whether current frag pos is in shadow
    float shadow = 0.0;
	
	// PCF
	vec2 texelSize = 1.0 / textureSize(shadow_map, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadow_map, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
	
	// keep the shadow at 0.0 when outside the far_plane region of the light's frustum
	if(projCoords.z > 1.0)
		shadow = 0.0;

    return shadow;
}

void main(){           
    vec3 color = texture(diffuse, tex_out).rgb;
    vec3 normal = normalize(norm_out);
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
    // calculate shadow map
    float shadow = ShadowCalculation(light_out);
    vec3 lighting = (1.0 - shadow) * (diffuse_var + specular) * color;  
    
	// remember to change to vec4
    FragColor = vec4(lighting, 1.0);
}