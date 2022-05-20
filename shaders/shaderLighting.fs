#version 330 core
layout (location = 0) out vec4 FragColor;
//layout (location = 1) out vec4 BrightColor; // may be used for bloom 

in VS_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 UV;
	vec4 FragPosLightSpace;
} fs_in;

vec4 result = vec4(0.0);

uniform sampler2D texture_diffuse0;
//uniform sampler2D texture_specular0;
uniform sampler2D shadowMap;

uniform vec3 viewPos;

struct Light_point
{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

uniform Light_point pointLights[32];

float calculateShadow()
{
	vec3 projCoords = fs_in.FragPosLightSpace.xyz / fs_in.FragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;
	float closestDepth = texture(shadowMap, projCoords.xy).r; 
	float currentDepth = projCoords.z;
	float bias = 0.0001;
	float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

	vec2 texelSize = 2.0 / textureSize(shadowMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}

	shadow /= 15;

	if(projCoords.z > 1.0)
		shadow = 0.0;

	return shadow;
}

vec3 calculatePointLight(Light_point light, vec3 norm, vec3 viewDir, vec3 tex_diffuse)
{
	vec3 ambient = light.ambient * tex_diffuse;

	vec3 lightDir = normalize(light.position - fs_in.FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * tex_diffuse;

	vec3 reflectDir = reflect(-lightDir, norm);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 2);
	vec3 specular = light.specular * spec;

	if (light.constant != 0 &&
		light.linear != 0 &&
		light.quadratic != 0)
	{
		float distance = distance(light.position, fs_in.FragPos);
		float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

		ambient *= attenuation; 
		diffuse *= attenuation;
		specular *= attenuation;
	}

	return vec3(diffuse + ambient + specular);
}

void main()
{
	vec3 result = vec3(0);

	vec3 norm = normalize(fs_in.Normal);
	vec3 viewDir = normalize(viewPos - fs_in.FragPos);
	vec3 tex = texture(texture_diffuse0, fs_in.UV).rgb;

	for (int i = 0; i < 32; i++) // caculate point lights
	{
		result += calculatePointLight(pointLights[i], norm, viewDir, tex);
	}

	//TODO: multiple shadow casters

	FragColor = vec4(result * (1 - calculateShadow()), 1);
}
