#version 330 core
// in vec2 TexCoord;
in vec3 Normal;
in vec3 Position;
in vec4 ParticleColor;

out vec4 FragColor;

// uniform sampler2D texture_diffuse1;
uniform samplerCube skybox;
uniform vec3 cameraPos;

void main ()
{
	float radio = 1.00f/1.33f;
	vec3 I = normalize(Position - cameraPos);
	vec3 R = refract(I, normalize(Normal), radio);
	FragColor = vec4(texture(skybox, R).rgb, 1.0f) * ParticleColor;
}