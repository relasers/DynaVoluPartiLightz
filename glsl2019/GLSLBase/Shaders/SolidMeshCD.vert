#version 450

in vec3 a_Position;
in vec3 a_Normal;

uniform mat4 u_ProjView;
uniform mat4 u_World;

varying vec3 v_pos;
varying vec3 v_norm;

void main()
{
	v_pos = (u_World * vec4(a_Position, 1.f)).xyz;
	//v_norm = normalize(mat3(u_World) * a_Normal);
	v_norm = normalize(u_World * vec4(a_Normal, 0.f)).xyz;
	gl_Position = u_ProjView * vec4(v_pos, 1);
}
