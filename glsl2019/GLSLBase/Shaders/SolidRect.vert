#version 450

in vec3 a_Position;
uniform mat4 u_ProjView;
uniform float u_Size;

void main()
{
	gl_Position = u_ProjView * vec4(a_Position * u_Size, 1);
}
