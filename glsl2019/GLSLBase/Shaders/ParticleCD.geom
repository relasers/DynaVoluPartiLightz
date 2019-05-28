#version 450

layout (points) in;
layout (points, max_vertices = 1) out;

in vec3 v_Pos[];
in vec3 v_Dir[];
in float f_Speed[];
in float f_CollideTime[];

out vec3 v_oPos;
out vec3 v_oDir;
out float f_oSpeed;
out float f_oCollideTime;

out vec3 outPos;
out vec3 outDir;
out float outSpeed;
out float outCollideTime;


void main()
{
	vec4 newpos = gl_in[0].gl_Position;

	v_oPos = outPos = v_Pos[0];
	v_oDir = outDir = v_Dir[0];
	f_oSpeed = outSpeed = f_Speed[0];
	f_oCollideTime = outCollideTime = f_CollideTime[0];

	gl_Position = newpos;
	EmitVertex();

    EndPrimitive();
}
