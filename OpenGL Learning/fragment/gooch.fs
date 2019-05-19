#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

vec3 CalcDirLight_G(vec3 direction, vec3 normal, vec3 viewDir);
vec3 CalcDirLight_I(vec3 direction, vec3 normal, vec3 viewDir);

uniform vec3 viewPos;
uniform vec3 direction;
uniform sampler2D texture_diffuse1;
uniform bool IorG;

void main()
{
	//属性
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 result;
	//第一阶段：定向光照
	if(!IorG)
		result = CalcDirLight_G(direction,norm,viewDir);
	else
		result = CalcDirLight_I(direction,norm,viewDir);

	FragColor =vec4(result, 1.0f);
}

vec3 CalcDirLight_G(vec3 direction, vec3 normal, vec3 viewDir)
{
	// Gooch shading
	vec3 k_cool = min(vec3(0.0f,0.0f,1.0f) + 0.2 * vec3(texture(texture_diffuse1, TexCoords)),1.0f);
	vec3 k_warm = min(vec3(1.0f,1.0f,0.0f) + 0.6 * vec3(texture(texture_diffuse1, TexCoords)),1.0f);

	vec3 lightDir = normalize(-direction);
	//漫反射着色
	float diff = dot(normal, lightDir);
	float it = (1.0f + diff) / 2.0f;
	//镜面光着色
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir),0.0f),32.0);
	//合并结果
	vec3 ambient = 0.2f * vec3(texture(texture_diffuse1, TexCoords));
	vec3 diffuse = it * k_cool + (1-it)*k_warm;
	vec3 specular = spec * vec3(texture(texture_diffuse1, TexCoords));

	return (ambient + diffuse +specular);
}

vec3 CalcDirLight_I(vec3 direction, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-direction);
	vec3 halfAngle = normalize(lightDir + viewDir);
	//漫反射着色
	float diff = dot(normal, lightDir);
	diff = 0.5 * diff + 0.5f;
	float NdotH = clamp(dot(halfAngle, normal), 0.0f, 1.0f);
	//镜面光着色
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(NdotH ,64.0);
	//合并结果
	vec3 ambient = 0.2f * vec3(texture(texture_diffuse1, TexCoords));
	vec3 diffuse = diff * vec3(texture(texture_diffuse1, TexCoords));
	vec3 specular = spec * vec3(texture(texture_diffuse1, TexCoords));

	return (ambient + diffuse +specular);
}