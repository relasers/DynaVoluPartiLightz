#version 430
layout( std140, binding= 4 ) buffer ParticlePosition
{
	vec4 Positions[];
};

uniform mat4 u_ProjView;
out vec4 v_WorldPosition;

void main()
{
	gl_Position = u_ProjView * Positions[gl_VertexID];
	v_WorldPosition = Positions[gl_VertexID];
}
