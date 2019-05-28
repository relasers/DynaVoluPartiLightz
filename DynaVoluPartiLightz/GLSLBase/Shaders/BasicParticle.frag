#version 450

layout(location=0) out vec4 FragColor;
in vec4 v_WorldPosition;
in vec4 v_ParticleColor;
void main()
{
	//FragColor = abs(v_WorldPosition/50.0f)*2;
	FragColor = v_ParticleColor;
}
