#version 450

in vec2 v_TexCoord;

uniform sampler2D u_Texture0;
uniform sampler2D u_Texture1;
uniform float u_Theta;
out vec4 FragColor;
void main()
{
	if (true == gl_FrontFacing) {
	  FragColor = texture(u_Texture0, v_TexCoord);
	}
	else {
	  FragColor = texture(u_Texture1, v_TexCoord);
	}
}
