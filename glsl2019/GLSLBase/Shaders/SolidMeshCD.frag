#version 450
layout(location = 0) out vec4 outpos;
layout(location = 1) out vec4 outnorm;

varying vec3 v_pos;
varying vec3 v_norm;

void main()
{
	outpos = vec4(v_pos, 1.0f);
	outnorm = vec4(normalize(v_norm), 1.0f);
}