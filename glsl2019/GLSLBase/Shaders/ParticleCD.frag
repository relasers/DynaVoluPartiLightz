#version 450

layout(location=0) out vec4 FragColor;

in vec3 v_oPos;
in vec3 v_oDir;
in float f_oSpeed;
in float f_oCollideTime;

void main()
{
	FragColor = vec4(v_oPos, 1);
}
