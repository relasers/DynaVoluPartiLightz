#version 450

in vec3 a_Position;
in vec3 a_Normal;
in vec4 a_TexCoord;
in vec4 a_VertColor;

uniform mat4 u_ProjView;
uniform mat4 u_World;
uniform vec3 u_CameraPos;

out vec4 v_Color;
out vec3 v_Normal;
out vec3 v_viewDir;
out vec4 v_WorldPosition;

void main()
{
	v_Color = a_VertColor;
	v_Color = vec4(a_Normal,1);
	v_Normal = a_Normal;
	v_WorldPosition = u_World * vec4(a_Position, 1.f);
	gl_Position = u_ProjView * v_WorldPosition;
	v_viewDir = normalize( a_Position.xyz - u_CameraPos );
}