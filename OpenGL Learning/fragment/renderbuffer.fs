#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

const float offset = 1.0 / 300.0;
void main()
{
	vec2 offsets[9] = vec2[](
		 vec2(-offset,  offset), // 左上
        vec2( 0.0f,    offset), // 正上
        vec2( offset,  offset), // 右上
        vec2(-offset,  0.0f),   // 左
        vec2( 0.0f,    0.0f),   // 中
        vec2( offset,  0.0f),   // 右
        vec2(-offset, -offset), // 左下
        vec2( 0.0f,   -offset), // 正下
        vec2( offset, -offset)  // 右下
        );
	float kernel_x[9] = float[](
        -1, 0, 1,
        -2,  0, 2,
        -1, 0, 1
    );
    float kernel_y[9] = float[](
        1, 2, 1,
        0,  0, 0,
        -1, -2, -1
    );
    vec3 sampleTex[9];
    for(int i=0;i<9;i++)
    	sampleTex[i] = vec3(texture(screenTexture,TexCoords.st + offsets[i]));
    vec3 col_x = vec3(0.0f);
    vec3 col_y = vec3(0.0f);
    for(int i=0;i<9;i++)
    {
    	col_x+= sampleTex[i] * kernel_x[i];
    	col_y+= sampleTex[i] * kernel_y[i];
    }

    FragColor = vec4(col_x * 0.5 + col_y * 0.5, 1.0);
    
} 