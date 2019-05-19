#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 aInstanceMatrix;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

void main()
{
	// vec4 worldPos = model * vec4(aPos ,1.0f);
	vec4 worldPos = aInstanceMatrix * vec4(aPos ,1.0f);
	FragPos = worldPos.xyz;
	TexCoords = aTexCoords;

	// mat3 normalMatrix = transpose(inverse(mat3(view * model)));
	mat3 normalMatrix = transpose(inverse(mat3(view * aInstanceMatrix)));
	Normal = normalMatrix * aNormal;

	gl_Position = projection * view * worldPos;
}