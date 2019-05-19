#version 330 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoord;

const int NUM_CASCADES = 3;

out VS_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec4 FragPosLightSpace[NUM_CASCADES];
	float ClipSpacePosZ;
}vs_out;

uniform mat4 gWVP;
uniform mat4 gLightWVP[NUM_CASCADES];
uniform mat4 model;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);
	vs_out.FragPos = vec3(model*vec4(position,1.0f));
	vs_out.Normal = transpose(inverse(mat3(model)))*normal;
	vs_out.TexCoords = texCoords;
	for (int i = 0; i<NUM_CASCADES;i++)
	{	
		vs_out.FragPosLightSpace[i] = gLightWVP[i] * vec4(vs_out.FragPos, 1.0f);
	}
	vs_out.ClipSpacePosZ = gl_Position.z;
}