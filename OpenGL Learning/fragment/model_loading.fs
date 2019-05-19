#version 440 core
#define NR_POINT_LIGHTS 2

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

struct DirLight{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight{
	vec3 position;
	float constant;
	float linear;
	float quadratic;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct SpotLight{
	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;
	float constant;
	float linear;
	float quadratic;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

//vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight lightDir, vec3 normal, vec3 FragPos, vec3 viewDir);
//vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 FragPos, vec3 viewDir);

void main()
{
	vec3 result = vec3(0.0f);
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	//FragColor += vec4(texture(texture_specular1, TexCoords));
	for(int i = 0; i < NR_POINT_LIGHTS; i++)
		result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
	FragColor = vec4(result, 1.0f);
} 

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 FragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - FragPos);
	//漫反射着色
	float diff = max(dot(normal, lightDir),0.0f);
	//镜面光着色
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir),0.0f),32);
	//衰减
	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
	//合并结果
	vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));
	vec3 diffuse = light.diffuse * vec3(texture(texture_diffuse1, TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoords));

	return (ambient + diffuse + specular) * attenuation;
}

// vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 FragPos, vec3 viewDir)
// {
// 	vec3 lightDir = normalize(light.position - FragPos);
// 	//漫反射着色
// 	float diff = max(dot(normal, lightDir),0.0f);
// 	//镜面光着色
// 	vec3 reflectDir = reflect(-lightDir, normal);
// 	float spec = pow(max(dot(viewDir, reflectDir),0.0f),32);
// 	//衰减
// 	float distance = length(light.position - FragPos);
// 	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
// 	//聚光灯强度
// 	float theta = dot(lightDir, normalize(-light.direction));
// 	float epsilon = light.cutOff - light.outerCutOff;
// 	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
// 	//合并结果
// 	vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));
// 	vec3 diffuse = light.diffuse * vec3(texture(texture_diffuse1, TexCoords));
// 	vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoords));

// 	return (ambient + diffuse + specular) * attenuation * intensity;
// }