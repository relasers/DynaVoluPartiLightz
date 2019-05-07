#version 330

uniform sampler2D u_Texture;

in vec2 v_TexPos;

layout(location=0) out vec4 FragColor;

void main()
{
	FragColor = texture(u_Texture, v_TexPos);
}
