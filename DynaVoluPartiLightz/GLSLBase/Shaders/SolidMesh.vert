#version 450

in vec3 a_Position;
in vec3 a_Normal;
in vec4 a_TexCoord;
in vec4 a_VertColor;

uniform mat4 u_ProjView;
uniform mat4 u_Model;
uniform vec3 u_CameraPos;

out vec4 v_Color;
out vec3 v_Normal;
out vec3 v_viewDir;
out vec4 v_WorldPosition;

void main()
{
	//v_FragPos = (u_Model * vec4(a_Position, 1.f)).xyz;
	v_Color = a_VertColor;
	v_Color = vec4(a_Normal,1);
	v_Normal = normalize(u_Model * vec4(a_Normal, 0.f )).xyz;
	v_WorldPosition = u_Model * vec4(a_Position, 1.f);
	gl_Position = u_ProjView * v_WorldPosition;
	v_viewDir = normalize( u_CameraPos.xyz - v_WorldPosition.xyz );
	//gl_Position = vec4(a_Position, 1.f);

}
