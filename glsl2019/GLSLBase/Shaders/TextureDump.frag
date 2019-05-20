#version 450

uniform sampler2D u_Texture;

in vec2 v_TexCoord;
layout(location=0) out vec4 FragColor;

void main()
{
	vec2 TexCoord = vec2(v_TexCoord.x, -v_TexCoord.y);
	FragColor = texture(u_Texture, TexCoord);
}
