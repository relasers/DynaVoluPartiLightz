#version 450

in vec3 a_Position;

out vec2 v_TexCoord;

void main()
{
	v_TexCoord = a_Position.xy * 0.5 + vec2(0.5);
	v_TexCoord.y = 1 - v_TexCoord.y;
	gl_Position = vec4(a_Position, 1);
}
