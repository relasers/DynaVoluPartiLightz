#version 450
layout(location=0) out vec4 FragColor;

varying vec3 v_Pos;

void main()
{
	FragColor = vec4(v_Pos, 1);
}