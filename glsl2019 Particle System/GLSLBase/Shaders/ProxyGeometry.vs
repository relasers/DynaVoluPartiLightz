#version 450

uniform mat4 u_ProjView;
in vec3 a_Position;

void main()
{
	gl_Position = u_ProjView * vec4(a_Position.x, 0, a_Position.y, 1);
}
