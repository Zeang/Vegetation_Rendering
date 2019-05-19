#version 330 core
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
// layout(location = 2) in vec2 vTexCoords;
layout(location = 2) in vec4 vColor;
layout(location = 3) in mat4 vInstanceMatrix;


// out vec2 TexCoords;
out vec3 Normal;
out vec3 Position;
out vec4 ParticleColor;

uniform mat4 projection;
uniform mat4 view;

void main ()
{
	// TexCoords = vTexCoords;
	Normal = mat3(transpose(inverse(vInstanceMatrix))) * vNormal;
	Position = vec3(vInstanceMatrix * vec4(vPos, 1.0f));
	ParticleColor = vColor;
	gl_Position = projection * view * vInstanceMatrix * vec4(vPos, 1.0f);
}