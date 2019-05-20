#version 450

in vec3 a_Position;
out vec2 v_TexCoord;

uniform mat4 u_ProjView;
uniform vec3 u_Axis;
uniform float u_Radius;
uniform float u_RunRate;

const float g_PI = 3.141592f;

mat4 rotate(float angle, vec3 axis) {
	const float a = angle;
	const float c = cos(a);
	const float s = sin(a);

	axis = normalize(axis);
	vec3 temp = vec3((1.0f - c) * axis);

	mat4 Rotate;
	Rotate[0][0] = c + temp[0] * axis[0];
	Rotate[0][1] = temp[0] * axis[1] + s * axis[2];
	Rotate[0][2] = temp[0] * axis[2] - s * axis[1];

	Rotate[1][0] = temp[1] * axis[0] - s * axis[2];
	Rotate[1][1] = c + temp[1] * axis[1];
	Rotate[1][2] = temp[1] * axis[2] + s * axis[0];

	Rotate[2][0] = temp[2] * axis[0] + s * axis[1];
	Rotate[2][1] = temp[2] * axis[1] - s * axis[0];
	Rotate[2][2] = c + temp[2] * axis[2];

	mat4 m = mat4(1, 0, 0, 0
				, 0, 1, 0, 0
				, 0, 0, 1, 0
				, 0, 0, 0, 1);

	mat4 Result;
	Result[0] = m[0] * Rotate[0][0] + m[1] * Rotate[0][1] + m[2] * Rotate[0][2];
	Result[1] = m[0] * Rotate[1][0] + m[1] * Rotate[1][1] + m[2] * Rotate[1][2];
	Result[2] = m[0] * Rotate[2][0] + m[1] * Rotate[2][1] + m[2] * Rotate[2][2];
	Result[3] = m[3];

	return Result;
}

mat4 translate(vec3 v) {
	mat4 m = mat4(1, 0, 0, 0
				, 0, 1, 0, 0
				, 0, 0, 1, 0
				, 0, 0, 0, 1);
	mat4 Result;
	Result[3] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2] + m[3];
	return Result;
}

void main()
{
	v_TexCoord = a_Position.xy + vec2(0.5);
	v_TexCoord.y = 1 - v_TexCoord.y;
	
	vec3 axis = normalize(u_Axis);
	vec3 vDiscriminant = normalize(cross(axis, vec3(0, 0, 1)));
	float radius = u_Radius-u_RunRate;

	if (0 < dot(a_Position, vDiscriminant) && u_RunRate > -0.75) {
		float a = vDiscriminant.x;
		float b = vDiscriminant.y;
		float c = vDiscriminant.z;

		float len = abs(a_Position.x*a + a_Position.y*b + a_Position.z*c) / sqrt(a*a + b*b + c*c);
		vec3 cross_point = a_Position - vDiscriminant * len;
		vec3 vCrossToPos = normalize(a_Position - cross_point);
		float theta = (len / radius);
		vec4 vPos = vec4(cross_point + vCrossToPos * len, 1);
		mat4 mtxTransform = rotate(-theta, u_Axis);
		vPos = mtxTransform * vPos;

		if (theta > g_PI * 0.7f){
			vPos = vec4(cross_point + ((radius - len) * vCrossToPos) + vec3(0, 0, 1.8f*radius), 1);
		}

		gl_Position = u_ProjView * vPos;
	}
	else {
		gl_Position = u_ProjView * vec4(a_Position, 1);
	}
}
