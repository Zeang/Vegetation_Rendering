#version 440 core
out vec4 FragColor;

in vec2 ourTexCoord;

uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;
uniform float ourAlpha;

void main()
{
	//FragColor = mix(texture(ourTexture1,ourTexCoord),texture(ourTexture2,vec2(1.0f - ourTexCoord.x, ourTexCoord.y)),ourAlpha);
	FragColor = vec4(1.0f,1.0f,0.0f,1.0f);
}