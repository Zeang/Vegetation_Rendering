#version 330 core

layout (location = 0) in vec3 Position;
// layout (location = 1) in vec2 TexCoord;
// layout (location = 2) in vec3 Normal;

uniform mat4 gWVP;
uniform mat4 model;

void main()
{
	gl_Position = gWVP * model * vec4(Position, 1.0f);
}