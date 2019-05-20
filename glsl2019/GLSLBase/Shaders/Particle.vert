#version 450

in vec3 a_Pos;
in vec3 a_Dir;
in float a_Speed;
in float a_CollideTime;

out vec3 v_Pos;
out vec3 v_Dir;
out float f_Speed;
out float f_CollideTime;

uniform mat4 u_ProjView;


void main()
{
	v_Pos = a_Pos;
	v_Dir = a_Dir;
	f_Speed = a_Speed;
	f_CollideTime = a_CollideTime;
	gl_Position = u_ProjView * vec4(a_Pos, 1);
}
