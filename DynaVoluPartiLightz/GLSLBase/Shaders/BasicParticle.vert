#version 430
layout( std140, binding= 0 ) buffer ParticlePosition
{
	vec4 Positions[];
};

layout( std140, binding= 1 ) buffer ParticleLightColor
{
	vec4 LightColor[];
};

uniform mat4 u_ProjView;
out vec4 v_WorldPosition;
out vec4 v_ParticleColor;
void main()
{
	gl_Position = u_ProjView * Positions[gl_VertexID];
	v_WorldPosition = Positions[gl_VertexID];
	v_ParticleColor = LightColor[gl_VertexID];
}
