#version 450

in vec3 a_Position;
in vec3 a_Normal;
in vec4 a_TexCoord;
in vec4 a_VertColor;

uniform mat4 u_ProjView;
uniform mat4 u_Model;


out vec4 Color;

void main()
{
	//v_FragPos = (u_Model * vec4(a_Position, 1.f)).xyz;
	Color = a_VertColor;
	Color = vec4(a_Normal,1);
	gl_Position = u_ProjView * u_Model * vec4(a_Position, 1.f);
}
