#version 440 core

in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

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

vec3 CalcPointLight(PointLight lightDir, vec3 normal, vec3 FragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 FragPos, vec3 viewDir);

uniform vec3 viewPos;
uniform vec3 CubeColor;
uniform bool sw;
uniform float spotLightAngel;
uniform PointLight pointLights;
uniform SpotLight spotLight;

void main()
{
	//属性
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);

	//第二阶段：点光源
	vec3 result = CalcPointLight(pointLights,norm,FragPos,viewDir);
	//第三阶段：聚光
	if(sw)
	{
		result += CalcSpotLight(spotLight,norm,FragPos,viewDir);
	}

	FragColor =vec4(result * CubeColor, 1.0f);
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
	vec3 ambient = light.ambient;
	vec3 diffuse = light.diffuse;
	vec3 specular = light.specular * spec;

	return (ambient + diffuse + specular) * attenuation;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 FragPos, vec3 viewDir)
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
	//聚光灯强度
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	//合并结果
	vec3 ambient = light.ambient;
	vec3 diffuse = light.diffuse;
	vec3 specular = light.specular * spec;

	return (ambient + diffuse + specular) * attenuation * intensity;
}