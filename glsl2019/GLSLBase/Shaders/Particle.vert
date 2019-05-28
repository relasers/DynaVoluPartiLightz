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
uniform vec3 u_CameraPos;

uniform int u_ClientWidth;
uniform int u_ClientHeight;

uniform sampler2D u_CDTexTure;
out vec4 v_TexelColor;

void main()
{
	v_Pos = a_Pos;
	v_Dir = a_Dir;
	f_Speed = a_Speed;
	f_CollideTime = a_CollideTime;

	ivec2 texel_base = ivec2(u_ClientWidth / 2,u_ClientHeight / 2);
	v_TexelColor = texelFetch(u_CDTexTure, texel_base, 0);

	gl_Position = u_ProjView * vec4(a_Pos, 1);
}
