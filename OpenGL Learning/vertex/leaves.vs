#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 aInstanceMatrix;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

// uniform mat4 model;
uniform sampler2D texture_diffuse;
uniform mat4 projection;
uniform mat4 view;
uniform vec3 Time;
uniform float WindSpeed;
uniform float WindIntensity;
uniform float WindWeight;
uniform float IfLeave;

vec3 RotateAboutAxis(vec3 NormalizedRotationAxis, float RotateAngle, vec3 PivotPoint, vec3 Position);
vec3 SampleGrassWind(vec3 Time,float WindIntensity, float WindWeight, float WindSpeed, vec3 AdditionWPO);
float AngleParameterFloat(float p);
vec3 AngleParameterVec(vec3 p);

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

	// move leaf position by wind
	if(IfLeave > 0.5f)
	{
		vec3 diffuse = texture(texture_diffuse, TexCoords).rgb;
		vec3 res = SampleGrassWind(Time, WindIntensity, WindWeight, WindSpeed, diffuse.xyz);
		gl_Position = projection * view * vec4(res + worldPos.xyz, 1.0f);
		FragPos = res + worldPos.xyz;
		// gAlbedoSpec.rbg = vec3(1.0f, 0.0f, 0.0f);
	}
}

vec3 RotateAboutAxis(vec3 NormalizedRotationAxis, float RotateAngle, vec3 PivotPoint, vec3 Position)
{
	// project position onto the rotation axis and find the closest point on the axis to Position
	vec3 ClosestPointOnAxis = PivotPoint + NormalizedRotationAxis * dot(NormalizedRotationAxis, Position - PivotPoint);
	// Construct orthogonal axes in the plane of the rotation
	vec3 UAxis = Position - ClosestPointOnAxis;
	vec3 VAxis = cross(NormalizedRotationAxis, UAxis);
	float CosAngle = cos(RotateAngle);
	float SinAngle = sin(RotateAngle);
	// rotate using the orthogonal axes
	vec3 R = UAxis * CosAngle + VAxis * SinAngle;
	// reconstruct the rotated world space position
	vec3 RotatedPosition = ClosestPointOnAxis + R;
	// convert from positon to a position offset
	return RotatedPosition - Position;
}

vec3 SampleGrassWind(vec3 Time,float WindIntensity, float WindWeight, float WindSpeed, vec3 AdditionWPO)
{
	vec3 rotateAxis = vec3(1.0f, 0.0f, 0.0f);
	vec3 temp1 = -0.5f * Time * WindSpeed;
	vec3 WPO_unit_1 = FragPos / 64.0f;
	vec3 WPO_unit_2 = FragPos / 200.0f;
	float angle_1 = AngleParameterFloat(temp1.y + WPO_unit_1.y);
	vec3 temp2 = AngleParameterVec(temp1 + WPO_unit_2);
	float angle_2 = distance(temp2, vec3(0.0f));
	float angle = angle_1 + angle_2;
	vec3 PivotPoint = AdditionWPO - vec3(0.0f, 0.0f, 10.0f);
	vec3 res = RotateAboutAxis(rotateAxis, angle, PivotPoint, AdditionWPO);
	res = res * WindIntensity * WindWeight;

	return res + AdditionWPO;
}

float AngleParameterFloat(float p)
{
	p = fract(p + 0.5f) * 2.0f - 1.0f;
	p = abs(p);
	p = (3.0f - 2.0f * p) * p * p;
	return p;
}

vec3 AngleParameterVec(vec3 p)
{
	p = fract(p + vec3(0.5f)) * 2.0f - vec3(1.0f);
	p = abs(p);
	p = (vec3(3.0f) - 2.0f * p) * p * p;

	return p;
}