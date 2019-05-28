#version 450

in vec3 a_Position;
in vec3 a_Normal;

uniform mat4 u_ProjView;
uniform mat4 u_World;

varying vec3 v_Pos;

void main()
{
	v_Pos = (u_World * vec4(a_Position, 1.f)).xyz;
	gl_Position = u_ProjView * vec4(v_Pos, 1);
}
