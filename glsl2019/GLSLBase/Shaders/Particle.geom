#version 450

layout (points) in;
layout (points, max_vertices = 1) out;

in vec3 v_Pos[];
in vec3 v_Dir[];
in float f_Speed[];
in float f_CollideTime[];

void main()
{
	vec4 newpos = gl_in[0].gl_Position;


	gl_Position = newpos;
	EmitVertex();

    EndPrimitive();
}
