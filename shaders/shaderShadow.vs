#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 vertexUV;
layout (location = 3) in ivec4 boneIds; 
layout (location = 4) in vec4 weights;

out VS_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 UV;
} vs_out;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 boneTransforms[100];

uniform bool animated;

void main()
{

	vec4 totalPosition = vec4(0.0f);
	vec3 localNormal;

	if (animated)
	{ 
		
		for(int i = 0 ; i < 4 ; i++)
		{
			if(boneIds[i] == -1) 
				continue;

			if(boneIds[i] >= 99) 
			{
				totalPosition = vec4(position,1.0f);
				break;
			}

			vec4 localPosition = boneTransforms[boneIds[i]] * vec4(position, 1.0f);
			
			if (boneTransforms[boneIds[i]] == mat4(0))
				totalPosition = vec4(position, 1.0f);
			else
				totalPosition += localPosition * weights[i];
			
			localNormal = mat3(boneTransforms[boneIds[i]]) * normal;
		}
	}
	else
	{
		totalPosition = vec4(position, 1);
		localNormal = normal;
	}

	vs_out.FragPos = vec3(model * totalPosition);
	vs_out.Normal = mat3(transpose(inverse(model))) * localNormal;
	vs_out.UV = vertexUV;

	gl_Position = projection * view * model * totalPosition;
}