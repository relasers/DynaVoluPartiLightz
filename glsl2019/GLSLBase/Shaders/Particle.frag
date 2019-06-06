#version 450

layout(location=0) out vec4 FragColor;

in vec3 v_Pos;
in vec3 v_Dir;
in float f_Speed;
in float f_CollideTime;

in vec4 v_TexelColor;


void main()
{
	if (0 < f_CollideTime)
		FragColor = vec4(1, 0, 0, 1);
	else
		FragColor = vec4(0.2f, 0, 0, 1);
}
