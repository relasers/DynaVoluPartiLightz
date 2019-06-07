#version 450

layout(location=0) out vec4 FragColor;

in vec3 vPos;
in vec3 vDir;
in float fSpeed;
in float fCollideTime;

uniform sampler2D u_ParticleTexTure;

void main()
{
	if (0 < fCollideTime)
		FragColor = vec4(1, 0, 0, 1);
	else
		FragColor = vec4(0, 0.2f, 0, 1);
}
