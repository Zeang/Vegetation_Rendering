#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 aInstanceMatrix;

vec3 SampleGrassWind(vec3 Time,float WindIntensity, float WindWeight, float WindSpeed, vec3 AdditionWPO, vec3 FragPos);
vec3 RotateAboutAxis(vec3 NormalizedRotationAxis, float RotateAngle, vec3 PivotPoint, vec3 Position);
float AngleParameterFloat(float p);
vec3 AngleParameterVec(vec3 p);

uniform mat4 lightSpaceMatrix;
uniform vec3 Time;
uniform float WindSpeed;
uniform float WindIntensity;
uniform float WindWeight;
uniform sampler2D diffuse;

void main()
{
	vec3 AdditionWPO = texture(diffuse, aTexCoords).rgb;
	vec4 FragPos = aInstanceMatrix * vec4(aPos, 1.0f);
	vec3 res = SampleGrassWind(Time, WindIntensity, WindWeight, WindSpeed, AdditionWPO, FragPos.rgb);
    gl_Position = lightSpaceMatrix * aInstanceMatrix * vec4(aPos + res, 1.0f);
}

vec3 SampleGrassWind(vec3 Time,float WindIntensity, float WindWeight, float WindSpeed, vec3 AdditionWPO, vec3 FragPos)
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